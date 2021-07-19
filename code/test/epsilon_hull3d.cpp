#include "test_utils.hpp"
#include "../geometry/epsilon/hull3d.hpp"
#include "../geometry/epsilon/hull3d_functions.hpp"
#include "../geometry/epsilon/hull3d_utils.hpp"
#include "../geometry/epsilon/point3d.hpp"

const string DATASET_FOLDER = "datasets/hull3d";
using P = Point3d;

inline namespace detail {

namespace fs = std::filesystem;
using hull_t = vector<vector<int>>;

inline double parse(string s) { return stod(s); }

inline double compute_ratio(double time, int N) { return 1e7 * time / (N * log2(N)); }

/**
 * Generate N random points inside the sphere of radius R or cube of side 2R
 */
auto random_points(int N, long R = 100, bool cube = false) {
    vector<P> points;
    while (N) {
        auto x = reald(-R, R)(mt);
        auto y = reald(-R, R)(mt);
        auto z = reald(-R, R)(mt);
        P p(x, y, z);
        if (cube || norm(p) <= R) {
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
void add_collinear_points(int N, vector<P>& points, long R = 100, bool cube = false) {
    int M = 0, S = points.size() - 1;
    assert(S >= 1);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt);
        double f = reald(-2, 3)(mt);
        if (i == j || abs(f) <= 1e-6 || abs(f - 1) <= 1e-6)
            continue;

        P p = f * points[i] + (1 - f) * points[j];

        if (cube ? p.boxed(P(-R, -R, -R), P(R, R, R)) : norm(p) <= R) {
            points.push_back(p), M++, S++;
        }
    }
}

/**
 * Generate N more points coplanar with those given or each other.
 */
void add_coplanar_points(int N, vector<P>& points, long R = 100, bool cube = false) {
    int M = 0, S = points.size() - 1;
    assert(S >= 2);
    while (M < N) {
        int i = intd(0, S)(mt), j = intd(0, S)(mt), k = intd(0, S)(mt);
        if (i == j || j == k || k == i || collinear(points[i], points[j], points[k]))
            continue;

        double f1 = reald(-2, 3)(mt);
        double f2 = reald(-2, 3)(mt);
        P p = f1 * points[i] + f2 * points[j] + (1 - f1 - f2) * points[k];

        if (cube ? p.boxed(P(-R, -R, -R), P(R, R, R)) : norm(p) <= R) {
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
    for (auto face : hull) {
        for (int& v : face)
            v++;
        s += format("f {}\n", face);
    }
    return s;
}

} // namespace detail

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
        simplify_hull(ans, points, 1e-12);
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

        auto counterexample = verify_hull(hull, points, P::deps);
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

void stress_test_quickhull3d() {
    const auto run = [&](int N, int L, int C, int I, long R = 50) {
        LOOP_FOR_DURATION_TRACKED (20s, now) {
            print_time(now, 20s, "stress test quickhull3d {} {} {} {}", N, L, C, I);

            auto points = random_points(N, R);
            add_coplanar_points(L, points, 2 * R);
            add_collinear_points(C, points, 2 * R);
            add_incident_points(I, points);

            auto hull = compute_hull(points);
            auto counterexample = verify_hull(hull, points, 300 * P::deps);
            if (counterexample) {
                auto [v, f, kind] = counterexample.value();
                if (kind == 0) {
                    print("error: point {} does not lie on plane of face {}\n", v, f);
                } else if (kind == 1) {
                    print("error: point {} can see plane of face {}\n", v, f);
                }
            }
            assert(!counterexample);
        }
    };

    run(170, 0, 0, 30);
    run(30, 0, 0, 0);
    run(8, 20, 0, 0);
    run(10, 50, 0, 0);
    run(25, 0, 0, 0);
    run(200, 0, 0, 0);
    run(90, 100, 10, 0);
    run(50, 900, 10, 40);
    run(9900, 0, 0, 100);
}

void scaling_test_quickhull3d() {
    vector<vector<stringable>> table;
    table.push_back({"N", "L", "C", "I", "P", "runs", "ratio", "time"});

    auto run = [&](int N, int L, int C, int I = 0, long R = 50) {
        int P = N + L + C + I;
        START_ACC(hull);

        LOOP_FOR_DURATION_TRACKED_RUNS (2s, now, runs) {
            print_time(now, 2s, "scaling test hull P={}", P);

            auto points = random_points(N, R);
            add_coplanar_points(L, points, R);
            add_collinear_points(C, points, R);
            assert(points.size() == unsigned(N + L + C + I));

            START(hull);
            auto hull = compute_hull(points);
            ADD_TIME(hull);
        }

        table.push_back({N, L, C, I, P, runs,
                         format("{:.2f}", compute_ratio(EACH_MS(hull, runs), P)),
                         FORMAT_EACH(hull, runs)});
    };

    run(400, 50, 50);
    run(300, 100, 100);
    run(800, 100, 100);
    run(600, 200, 200);
    run(400, 300, 300);
    run(1500, 2500, 2500);
    run(1200, 4000, 4000);
    run(4500, 2500, 2500);
    run(3500, 7500, 7500);
    run(9400, 3000, 3000);
    run(5000, 2500, 2500);
    run(15000, 2500, 2500);
    run(15000, 2500, 2500);
    run(100000, 0, 0);
    run(10000, 90000, 0);
    run(1'000'000, 0, 0);
    run(10'000, 900'000, 90'000);

    print_time_table(table, "Quickhull3d");
}

int main() {
    mt.seed(87);
    RUN_BLOCK(dataset_test_quickhull3d());
    RUN_BLOCK(stress_test_quickhull3d());
    RUN_BLOCK(scaling_test_quickhull3d());
    return 0;
}
