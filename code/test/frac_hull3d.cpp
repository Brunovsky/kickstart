#include "../geometry/frac/frac_hull3d.hpp"

#include <filesystem>

#include "../formatting.hpp"
#include "../geometry/frac/frac_hull3d_functions.hpp"
#include "../lib/anynum.hpp"
#include "../geometry/frac/frac_hull3d_utils.hpp"
#include "test_utils.hpp"

namespace fs = std::filesystem;

/**
 * We test using verify_hull and debug with visualization from the generated OBJ files
 * on e.g. blender. You can change face indexing on qh.extract_faces(s = 0|1).
 */

const string DATASET_FOLDER = "datasets/hull3d";

using F = bfrac;
using P = Point3d<F>;
using Pln = Plane<F>;

inline namespace {

inline double compute_ratio(double time, int N) { return 1e4 * time / (N * log2(N)); }

/**
 * Generate N random points inside the sphere of radius R or cube of side 2R
 */
auto random_points(int N, long R = 100, long maxd = 4, bool cube = false) {
    unordered_set<P> pointset;
    while (N) {
        auto x = uniform_gen<F>(-R, R, maxd);
        auto y = uniform_gen<F>(-R, R, maxd);
        auto z = uniform_gen<F>(-R, R, maxd);
        P p(x, y, z);
        if (!pointset.count(p)) {
            if (cube || p.norm2() <= R * R) {
                pointset.insert(move(p)), N--;
            }
        }
    }
    vector<P> points(begin(pointset), end(pointset));
    return points;
}

/**
 * Generate N more points collinear with those given or each other.
 */
void add_collinear_points(int N, vector<P>& points, long R = 100, long maxd = 4,
                          bool cube = false) {
    int M = 0, S = points.size() - 1;
    unordered_set<P> pointset(begin(points), end(points));
    assert(S >= 1);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt);
        auto f = uniform_gen<F>(-2, 3, maxd);
        if (i == j || f == 0 || f == 1)
            continue;

        P p = f * points[i] + (1 - f) * points[j];

        if (!pointset.count(p)) {
            if (cube ? p.boxed(P(-R, -R, -R), P(R, R, R)) : p.norm2() <= R * R) {
                pointset.insert(p);
                points.push_back(p), M++, S++;
            }
        }
    }
}

/**
 * Generate N more points coplanar with those given or each other.
 */
void add_coplanar_points(int N, vector<P>& points, long R = 100, long maxd = 4,
                         bool cube = false) {
    int M = 0, S = points.size() - 1;
    unordered_set<P> pointset(begin(points), end(points));
    assert(S >= 2);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt), k = intd(0, S)(mt);
        if (i == j || j == k || k == i || collinear(points[i], points[j], points[k]))
            continue;

        auto f1 = uniform_gen<F>(-2, 3, maxd);
        auto f2 = uniform_gen<F>(-2, 3, maxd);
        P p = f1 * points[i] + f2 * points[j] + (1 - f1 - f2) * points[k];

        if (!pointset.count(p)) {
            if (cube ? p.boxed(P(-R, -R, -R), P(R, R, R)) : p.norm2() <= R * R) {
                pointset.insert(p);
                points.push_back(move(p)), M++, S++;
            }
        }
    }
}

string format_header(const vector<P>& points, int t = -1) {
    int N = points.size();
    string s;
    s += t == -1 ? "" : format("# Problem {}\n", t);
    s += format("# {} points\n", N);
    if (N <= 1000) {
        auto dups = find_incident(points);
        s += format("#  Incident: {}\n", dups ? "yes: " + to_string(*dups) : "no"s);
    }
    if (N <= 30) {
        auto trip = find_collinear(points);
        s += format("# Collinear: {}\n", trip ? "yes: " + to_string(*trip) : "no"s);
    }
    if (N <= 25) {
        auto quad = find_coplanar(points);
        s += format("#  Coplanar: {}\n", quad ? "yes: " + to_string(*quad) : "no"s);
    }
    return s;
}

string format_points(const vector<P>& points) {
    string s;
    for (const auto& p : points) {
        s += format("v {} {} {}\n", double(p.x), double(p.y), double(p.z));
    }
    return s;
}

string format_hull(const hull_t& hull) {
    string s = format("# {} faces\n", hull.size());
    for (const auto& face : hull) {
        s += format("f {}\n", face);
    }
    return s;
}

} // namespace

struct frac_quickhull3d_dataset_test_t {
    string name, comment;
    vector<P> points;
    hull_t ans;

    void read(string name, istream& in) {
        this->name = name;
        string s, line;
        points = {P::zero()};
        while (getline(in, line)) {
            if (line.empty())
                continue;

            stringstream ss(line);
            ss >> ws >> s;

            if (s == "#") {
                comment += line + '\n';
            } else if (s == "v") {
                string x, y, z;
                ss >> ws >> x >> ws >> y >> ws >> z;
                points.emplace_back(stobfrac(x), stobfrac(y), stobfrac(z));
            } else if (s == "f") {
                vector<int> face;
                int v;
                while (ss >> v) {
                    face.push_back(v);
                }
                ans.emplace_back(move(face));
            }
        }
        simplify_hull(ans, points);
        canonicalize_hull(ans);
    }

    void write() { print("== {}\n{}", name, comment); }

    void run() {
        auto hull = compute_hull(points, 1);
        canonicalize_hull(hull);

        print("hull area: {}\n", area_hull(hull, points));
        print("hull volume: {}\n", volume_hull(hull, points));
        if (hull != ans) {
            print("-- quickhull error on {}\n", name);
            print("     got: {}\n", hull);
            print("expected: {}\n", ans);
        }

        auto counterexample = verify_hull(hull, points, 1);
        if (counterexample) {
            auto [v, f, kind] = counterexample.value();
            clear_line(), print("Incorrect convex hull\n");
            if (kind == 0) {
                print("ce: point {} does not lie on plane of face {}\n", v, f);
            } else if (kind == 1) {
                print("ce: point {} can see plane of face {}\n", v, f);
            }
        }
    }
};

void dataset_test_frac_quickhull3d() {
    fs::path dir = DATASET_FOLDER;
    for (auto& entry : fs::directory_iterator(dir)) {
        ifstream file(entry.path());
        assert(file.is_open());
        frac_quickhull3d_dataset_test_t test;
        test.read(entry.path(), file);
        test.write();
        test.run();
        print("\n");
    }
}

void stress_test_frac_quickhull3d(int T = 300, int N = 10, long R = 60, long maxd = 10) {
    auto dir = fs::temp_directory_path();
    print("Temp directory: {}\n", dir);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test frac_quickhull3d");
        auto file = dir / fs::path(format("hull3d-stress-{}.obj", t));
        ofstream out(file.string());

        auto points = random_points(N, R, maxd);
        points.insert(begin(points), P::zero());
        add_coplanar_points(2 * N, points, 2 * R, maxd);
        add_collinear_points(N, points, 2 * R, maxd);
        out << format_header(points) << format_points(points);

        auto hull = compute_hull(points, 1);
        out << format_hull(hull);

        auto counterexample = verify_hull(hull, points, 1);
        if (counterexample) {
            auto [v, f, kind] = counterexample.value();
            clear_line();
            print("Incorrect convex hull, check file {}\n", file.string());
            if (kind == 0) {
                print("ce: point {} does not lie on plane of face {}\n", v, f);
            } else if (kind == 1) {
                print("ce: point {} can see plane of face {}\n", v, f);
            }
        }
    }
}

void scaling_test_frac_quickhull3d_run(int T, int N, int L, int C, int I = 0, long R = 50,
                                       long maxd = 10) {
    // N points + L collinear + C coplanar + I incident
    if (T == 0)
        return;

    START_ACC(frac_quickhull3d);
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "scaling test");
        auto points = random_points(N, R, maxd);
        add_coplanar_points(L, points, R, maxd);
        add_collinear_points(C, points, R, maxd);
        assert(points.size() == uint(N + L + C + I));

        START(frac_quickhull3d);
        auto hull = compute_hull(points, 0);
        ADD_TIME(frac_quickhull3d);
    }

    int all = N + L + C + I;
    auto each = 1.0 * TIME_MS(frac_quickhull3d) / T;
    print(" {:>8}ms -- {:>7.1f}ms each -- {:7.1f} ratio -- x{:<6}  P={:<6} "
          "(N={},L={},C={},I={})\n",
          TIME_MS(frac_quickhull3d), each, compute_ratio(each, all), T, all, N, L, C, I);
}

void scaling_test_frac_quickhull3d(double F = 1.0) {
    scaling_test_frac_quickhull3d_run(int(F * 200), 40, 5, 5);
    scaling_test_frac_quickhull3d_run(int(F * 200), 30, 10, 10);
    scaling_test_frac_quickhull3d_run(int(F * 50), 80, 10, 10);
    scaling_test_frac_quickhull3d_run(int(F * 50), 60, 20, 20);
    scaling_test_frac_quickhull3d_run(int(F * 50), 40, 30, 30);
    scaling_test_frac_quickhull3d_run(int(F * 25), 150, 25, 25);
    scaling_test_frac_quickhull3d_run(int(F * 25), 120, 40, 40);
    scaling_test_frac_quickhull3d_run(int(F * 10), 450, 25, 25);
    scaling_test_frac_quickhull3d_run(int(F * 10), 350, 75, 75);
    scaling_test_frac_quickhull3d_run(int(F * 4), 940, 30, 30);
    scaling_test_frac_quickhull3d_run(int(F * 4), 500, 250, 250);
}

int main() {
    RUN_BLOCK(dataset_test_frac_quickhull3d());
    RUN_BLOCK(stress_test_frac_quickhull3d());
    RUN_BLOCK(scaling_test_frac_quickhull3d());
    return 0;
}
