#include "test_utils.hpp"
#include "../hash.hpp"
#include "../geometry/hull3d.hpp"
#include "../geometry/point3d.hpp"
#include "../geometry/hull3d_functions.hpp"
#include "../geometry/hull3d_utils.hpp"

namespace fs = std::filesystem;
const string DATASET_FOLDER = "datasets/hull3d";
using hull_t = vector<vector<int>>;
using NumT = long;
using P = Point3d<NumT>;

inline namespace {

inline double compute_ratio(double time, int N) { return 1e7 * time / (N * log2(N)); }

/**
 * Generate N random points inside the sphere of radius R or cube of side 2R
 */
auto random_points(int N, long R = 50, bool cube = false) {
    vector<P> points;
    while (N) {
        long x = longd(-R, R)(mt);
        long y = longd(-R, R)(mt);
        long z = longd(-R, R)(mt);
        P p(x, y, z);
        if (cube || p.norm() <= R) {
            points.push_back(move(p)), N--;
        }
    }
    return points;
}

void add_incident_points(int N, vector<P>& points) {
    int S = points.size() - 1;
    assert(S >= 0);
    while (N--) {
        auto point = points[intd(0, S++)(mt)];
        points.push_back(point);
    }
}

/**
 * Generate N more points collinear with those given or each other.
 */
void add_collinear_points(int N, vector<P>& points, long R = 50, bool cube = false) {
    int M = 0, S = points.size() - 1;
    assert(S >= 1);
    P box = P(R, R, R);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt);
        long f = longd(-5, 5)(mt);
        if (i == j || f == 0 || f == 1)
            continue;

        P p = interpolate(points[i], points[j], f);

        if (cube ? p.boxed(-box, box) : p.norm() <= R) {
            points.push_back(p), M++, S++;
        }
    }
}

/**
 * Generate N more points coplanar with those given or each other.
 */
void add_coplanar_points(int N, vector<P>& points, long R = 50, bool cube = false) {
    int M = 0, S = points.size() - 1;
    assert(S >= 2);
    P box = P(R, R, R);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt), k = intd(0, S)(mt);
        if (i == j || j == k || k == i || collinear(points[i], points[j], points[k]))
            continue;

        long f1 = longd(-5, 5)(mt);
        long f2 = longd(-5, 5)(mt);
        long f3 = 1 - f1 - f2;
        P p = f1 * points[i] + f2 * points[j] + f3 * points[k];
        if ((f1 == 0) + (f2 == 0) + (f3 == 0) >= 2)
            continue;

        if (cube ? p.boxed(-box, box) : p.norm() <= R) {
            points.push_back(p), M++, S++;
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
    return s;
}

string format_points(const vector<P>& points) {
    string s;
    for (const auto& p : points) {
        s += format("v {} {} {}\n", p.x, p.y, p.z);
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

struct quickhull3d_dataset_test_t {
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
                points.emplace_back(stoll(x), stoll(y), stoll(z));
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

void dataset_test_quickhull3d() {
    fs::path dir = DATASET_FOLDER;
    for (auto& entry : fs::directory_iterator(dir)) {
        ifstream file(entry.path());
        assert(file.is_open());
        quickhull3d_dataset_test_t test;
        test.read(entry.path(), file);
        test.write();
        test.run();
        print("\n");
    }
}

void stress_test_quickhull3d(int T = 300, int N = 10, long R = 50) {
    auto dir = fs::temp_directory_path();
    print("Temp directory: {}\n", dir);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test quickhull3d");
        auto file = dir / fs::path(format("hull3d-stress-{}.obj", t));
        ofstream out(file.string());

        auto points = random_points(N, R);
        points.insert(begin(points), P::zero());
        add_coplanar_points(2 * N, points, 2 * R);
        add_collinear_points(N, points, 2 * R);
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

void scaling_test_quickhull3d_run(int T, int N, int L, int C, int I = 0, long R = 50) {
    // N points + L collinear + C coplanar + I incident
    if (T == 0)
        return;

    START_ACC(quickhull3d);
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "scaling test");
        auto points = random_points(N, R);
        add_coplanar_points(L, points, R);
        add_collinear_points(C, points, R);
        assert(points.size() == uint(N + L + C + I));

        START(quickhull3d);
        auto hull = compute_hull(points, 0);
        ADD_TIME(quickhull3d);
    }

    int all = N + L + C + I;
    auto each = 1.0 * TIME_MS(quickhull3d) / T;
    print(" {:>8}ms -- {:>7.1f}ms each -- {:7.1f} ratio -- x{:<6}  P={:<6} "
          "(N={},L={},C={},I={})\n",
          TIME_MS(quickhull3d), each, compute_ratio(each, all), T, all, N, L, C, I);
}

void scaling_test_quickhull3d(double F = 1.0) {
    scaling_test_quickhull3d_run(int(F * 200), 40, 5, 5);
    scaling_test_quickhull3d_run(int(F * 200), 30, 10, 10);
    scaling_test_quickhull3d_run(int(F * 50), 80, 10, 10);
    scaling_test_quickhull3d_run(int(F * 50), 60, 20, 20);
    scaling_test_quickhull3d_run(int(F * 50), 40, 30, 30);
    scaling_test_quickhull3d_run(int(F * 25), 150, 25, 25);
    scaling_test_quickhull3d_run(int(F * 25), 120, 40, 40);
    scaling_test_quickhull3d_run(int(F * 10), 450, 25, 25);
    scaling_test_quickhull3d_run(int(F * 10), 350, 75, 75);
    scaling_test_quickhull3d_run(int(F * 4), 940, 30, 30);
    scaling_test_quickhull3d_run(int(F * 4), 500, 250, 250);
}

int main() {
    RUN_BLOCK(dataset_test_quickhull3d());
    RUN_BLOCK(stress_test_quickhull3d());
    RUN_BLOCK(scaling_test_quickhull3d());
    return 0;
}
