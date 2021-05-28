#include "test_utils.hpp"
#include "../geometry/point3d.hpp"
#include "../geometry/hull3d.hpp"
#include "../geometry/hull3d_functions.hpp"
#include "../geometry/hull3d_utils.hpp"

using NumT = long;
using P = Point3d<NumT>;
const string DATASET_FOLDER = "datasets/hull3d";

inline namespace detail {

namespace fs = std::filesystem;
using hull_t = vector<vector<int>>;

inline NumT parse(string s) { return stoll(s); }
// inline string convert(NumT n) { to_string(n >= 0 ? long(n) : -long(-n)); }
inline string convert(NumT n) { return to_string(n); }

inline auto compute_ratio(double time, int N) {
    return format("{:7.1f} ratio", 1e7 * time / (N * log2(N)));
}

/**
 * Generate N random points inside the sphere of radius R or cube of side 2R
 */
auto random_points(int N, int R = 100, bool cube = true) {
    unordered_set<P> pointset;
    P box(R, R, R);
    while (N) {
        int x = intd(-R, R)(mt);
        int y = intd(-R, R)(mt);
        int z = intd(-R, R)(mt);
        P p(x, y, z);
        if (!pointset.count(p)) {
            if (cube ? p.boxed(-box, box) : p.norm() <= R) {
                pointset.insert(p), N--;
            }
        }
    }
    vector<P> points(begin(pointset), end(pointset));
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
void add_collinear_points(int N, vector<P>& points, int R = 100, bool cube = true) {
    int S = points.size() - 1;
    assert(S >= 1);
    unordered_set<P> pointset(begin(points), end(points));
    P box(R, R, R);
    while (N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt);
        if (i == j)
            continue;

        int f = intd(-5, 5)(mt);
        P p = interpolate(points[i], points[j], f);

        if (!pointset.count(p)) {
            if (cube ? p.boxed(-box, box) : p.norm() <= R) {
                pointset.insert(p);
                points.push_back(p), N--, S++;
            }
        }
    }
}

/**
 * Generate N more points coplanar with those given or each other.
 */
void add_coplanar_points(int N, vector<P>& points, int R = 100, bool cube = true) {
    int S = points.size() - 1;
    assert(S >= 2);
    unordered_set<P> pointset(begin(points), end(points));
    P box(R, R, R);
    while (N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt), k = intd(0, S)(mt);
        if (i == j || j == k || k == i || collinear(points[i], points[j], points[k]))
            continue;

        int f1 = intd(-5, 5)(mt);
        int f2 = intd(-5, 5)(mt);
        int f3 = 1 - f1 - f2;
        P p = f1 * points[i] + f2 * points[j] + f3 * points[k];

        if (!pointset.count(p)) {
            if (cube ? p.boxed(-box, box) : p.norm() <= R) {
                pointset.insert(p);
                points.push_back(p), N--, S++;
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
    return s;
}

string format_points(const vector<P>& points) {
    string s;
    for (const auto& p : points) {
        s += format("v {} {} {}\n", convert(p.x), convert(p.y), convert(p.z));
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

} // namespace detail

inline namespace dataset_testing_hull3d {

struct quickhull3d_dataset_test_t {
    string name, comment;
    vector<P> points;
    hull_t ans;

    void read(string name, istream& in) {
        this->name = name;
        string s, line;
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
                points.emplace_back(parse(x), parse(y), parse(z));
            } else if (s == "f") {
                vector<int> face;
                int v;
                while (ss >> v) {
                    v--, face.push_back(v);
                }
                ans.emplace_back(move(face));
            }
        }
        simplify_hull(ans, points);
        canonicalize_hull(ans);
    }

    void write() { print("== {}\n{}", name, comment); }

    void run() {
        auto hull = compute_hull(points);
        canonicalize_hull(hull);

        print("hull area: {}\n", area_hull(hull, points));
        print("hull volume: {}\n", volume_hull(hull, points));
        if (hull != ans) {
            print("-- quickhull error on {}\n", name);
            print("     got: {}\n", hull);
            print("expected: {}\n", ans);
        }

        auto counterexample = verify_hull(hull, points);
        if (counterexample) {
            auto [v, f, kind] = counterexample.value();
            printcl("Incorrect convex hull\n");
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

} // namespace dataset_testing_hull3d

inline namespace stress_testing_hull3d {

fs::path tmpfile;

void stress_test_quickhull3d_run(int N, int L, int C, int I, long R = 100) {
    LOOP_FOR_DURATION_TRACKED(4s, now) {
        print_time(now, 4s, 50ms, "stress test quickhull3d");

        auto points = random_points(N, R);
        add_coplanar_points(L, points, 2 * R);
        add_collinear_points(C, points, 2 * R);
        add_incident_points(I, points);

        ofstream out(tmpfile.string());
        out << format_header(points) << format_points(points);

        auto hull = compute_hull(points);
        out << format_hull(hull);

        auto counterexample = verify_hull(hull, points);
        if (counterexample) {
            auto [v, f, kind] = counterexample.value();
            printcl("Incorrect convex hull, check file {}\n", tmpfile.string());
            if (kind == 0) {
                print("ce: point {} does not lie on plane of face {}\n", v, f);
            } else if (kind == 1) {
                print("ce: point {} can see plane of face {}\n", v, f);
            }
            exit(1);
        }
    }
}

void stress_test_quickhull3d(double F = 1.0) {
    auto tmpdir = fs::temp_directory_path() / fs::path("hull3d");
    fs::create_directory(tmpdir);
    tmpfile = tmpdir / fs::path("hull3d-stress.obj");
    print("Temp file: {}\n", tmpfile);

    stress_test_quickhull3d_run(int(F * 500), 8, 20, 0, 0);
    stress_test_quickhull3d_run(int(F * 500), 10, 50, 0, 0);
    stress_test_quickhull3d_run(int(F * 500), 25, 0, 0, 0);
    stress_test_quickhull3d_run(int(F * 500), 200, 0, 0, 0);
    stress_test_quickhull3d_run(int(F * 500), 170, 0, 0, 30);
    stress_test_quickhull3d_run(int(F * 500), 90, 100, 10, 0);
    stress_test_quickhull3d_run(int(F * 400), 50, 900, 10, 40);
    stress_test_quickhull3d_run(int(F * 100), 9900, 0, 0, 100);
}

} // namespace stress_testing_hull3d

inline namespace scaling_testing_hull3d {

void scaling_test_quickhull3d_run(int N, int L, int C, int I = 0, long R = 200) {
    START_ACC(hull);

    LOOP_FOR_DURATION_TRACKED_RUNS(2s, now, runs) {
        print_time(now, 2s, 50ms, "scaling test hull");

        auto points = random_points(N, R);
        add_coplanar_points(L, points, R);
        add_collinear_points(C, points, R);
        assert(points.size() == uint(N + L + C + I));

        START(hull);
        auto hull = compute_hull(points);
        ADD_TIME(hull);
    }

    int all = N + L + C + I;
    print(" {:>8.2f}ms/1 -- {} -- x{:<6}  P={:<6} (N={},L={},C={},I={})\n",
          EACH_MS(hull, runs), compute_ratio(EACH_MS(hull, runs), all), runs, all, N, L,
          C, I);
}

void scaling_test_quickhull3d() {
    scaling_test_quickhull3d_run(400, 50, 50);
    scaling_test_quickhull3d_run(300, 100, 100);
    scaling_test_quickhull3d_run(800, 100, 100);
    scaling_test_quickhull3d_run(600, 200, 200);
    scaling_test_quickhull3d_run(400, 300, 300);
    scaling_test_quickhull3d_run(1500, 2500, 2500);
    scaling_test_quickhull3d_run(1200, 4000, 4000);
    scaling_test_quickhull3d_run(4500, 2500, 2500);
    scaling_test_quickhull3d_run(3500, 7500, 7500);
    scaling_test_quickhull3d_run(9400, 3000, 3000);
    scaling_test_quickhull3d_run(5000, 2500, 2500);
    scaling_test_quickhull3d_run(15'000, 2500, 2500);
    scaling_test_quickhull3d_run(15'000, 2500, 2500);
    scaling_test_quickhull3d_run(100'000, 0, 0);
    scaling_test_quickhull3d_run(10'000, 90'000, 0);
    scaling_test_quickhull3d_run(400'000, 0, 0);
    scaling_test_quickhull3d_run(10'000, 300'000, 90'000);
}

} // namespace scaling_testing_hull3d

int main() {
    RUN_BLOCK(dataset_test_quickhull3d());
    RUN_BLOCK(stress_test_quickhull3d());
    RUN_BLOCK(scaling_test_quickhull3d());
    return 0;
}
