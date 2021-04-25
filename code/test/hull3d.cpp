#define DEBUG_HULL3D
#include "../geometry/hull3d.hpp"

#include <filesystem>

#include "../formatting.hpp"
#include "../generators/frac.hpp"
#include "../geometry/hull3d_utils.hpp"
#include "test_utils.hpp"

namespace fs = std::filesystem;

// *****

const string DATASET_FOLDER = "datasets/hull3d";

using F = bfrac;
using P = Point3d<F>;
using Pln = Plane<F>;
using quickhull3d = frac_quickhull3d<F>;

template struct frac_quickhull3d<F>;

/**
 * Generate N random points inside the sphere of radius R
 */
auto random_points(uint N, long R = 100, long maxd = 4) {
    vector<P> points;
    while (points.size() < N) {
        bfrac x = gen_bfrac(-R, R, maxd);
        bfrac y = gen_bfrac(-R, R, maxd);
        bfrac z = gen_bfrac(-R, R, maxd);
        P point(x, y, z);
        if (point.norm2() <= R * R) {
            points.push_back(point);
        }
    }
    return points;
}

optional<array<int, 4>> find_coplanar(const vector<P>& points) {
    unordered_map<Pln, array<int, 3>, PlaneHasher> planes;
    for (int u = 0, N = points.size(); u < N; u++) {
        for (int v = u + 1; v < N; v++) {
            for (int w = v + 1; w < N; w++) {
                Pln plane(points[u], points[v], points[w]);
                plane.normalize();
                if (plane.is_degenerate())
                    continue;
                if (!planes.emplace(plane, array<int, 3>{u, v, w}).second) {
                    for (int x : planes.at(plane))
                        if (x != u && x != v && x != w)
                            return {{x, u, v, w}};
                    assert(false);
                }
            }
        }
    }
    return {};
}

optional<array<int, 3>> find_collinear(const vector<P>& points) {
    for (int u = 0, N = points.size(); u < N; u++) {
        for (int v = u + 1; v < N; v++) {
            for (int w = v + 1; w < N; w++) {
                if (collinear(points[u], points[v], points[w]))
                    return {{u, v, w}};
            }
        }
    }
    return {};
}

optional<array<int, 2>> find_incident(const vector<P>& points) {
    unordered_map<P, int, Hasher3d> pointmap;
    for (int u = 0, N = points.size(); u < N; u++) {
        if (pointmap.count(points[u]))
            return {{u, pointmap[points[u]]}};
        pointmap.emplace(points[u], u);
    }
    return {};
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
        s += format("v {} {} {}\n", to_decimal(p.x), to_decimal(p.y), to_decimal(p.z));
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

struct quickhull3d_dataset_test_t {
    string name, comment;
    vector<P> points;
    hull_t hull;

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
                points.emplace_back(stobfrac(x), stobfrac(y), stobfrac(z));
            } else if (s == "f") {
                vector<int> face;
                int v;
                while (ss >> v) {
                    face.push_back(v);
                }
                hull.emplace_back(move(face));
            }
        }
        simplify_hull(hull, points, 1);
        canonicalize_hull(hull);
    }

    void write() { print("== {}\n{}{}", name, comment, format_header(points)); }

    void run() {
        quickhull3d qh(points);
        qh.compute();
        auto got = qh.extract_hull(1);
        simplify_hull(got, points, 1);
        canonicalize_hull(got);

        if (got != hull) {
            print("-- quickhull error on {}\n", name);
            print("     got: {}\n", got);
            print("expected: {}\n", hull);
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

void stress_test_quickhull3d(int T = 1000, int N = 300, long R = 400, long maxd = 20) {
    auto dir = fs::temp_directory_path();
    print("Temp directory: {}\n", dir);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test quickhull3d"), print("\n");
        auto file = dir / fs::path(format("hull3d-stress-{}.obj", t));
        ofstream out(file.string());

        auto points = random_points(N, R, maxd);
        out << format_header(points) << format_points(points);

        quickhull3d qh(points);
        bool ok = qh.compute();
        assert(ok);
        auto hull = qh.extract_hull(1);

        out << format_hull(hull);
    }
}

int main() {
    mt.seed(73);
    RUN_BLOCK(stress_test_quickhull3d());
    RUN_BLOCK(dataset_test_quickhull3d());
    return 0;
}
