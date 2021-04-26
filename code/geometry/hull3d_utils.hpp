#ifndef HULL3D_UTILS_HPP
#define HULL3D_UTILS_HPP

#include "hash.hpp"
#include "point3d.hpp" // Point3d, Plane

// *****

inline namespace hull3d_utils {

/**
 * Returns [v, f, kind] if there is a counterexample for this hull's correctness.
 * kind=0 ==> v is assigned to face f but isn't in its plane.
 * kind=1 ==> v can see face f (from outside).
 */
optional<tuple<int, int, int>> verify_hull(const hull_t& hull,
                                           const vector<Point3d>& points, int s,
                                           double eps) {
    int H = hull.size(), N = points.size();
    vector<Plane> planes;
    vector<unordered_set<int>> vfaces(N);
    for (int f = 0; f < H; f++) {
        const auto& face = hull[f];
        int u = face[0] - s, v = face[1] - s, w = face[2] - s;
        planes.emplace_back(points[u], points[v], points[w]);
        for (int v : face) {
            vfaces[v - s].insert(f);
        }
    }
    for (int v = 0; v < N; v++) {
        for (int f : vfaces[v]) {
            if (planes[f].planeside(points[v], eps) != 0) {
                return {{v + s, f, 0}};
            }
        }
    }
    for (int v = 0; v < N; v++) {
        for (int f = 0; f < H; f++) {
            if (!vfaces[v].count(f) && planes[f].planeside(points[v], eps) == 1) {
                return {{v + s, f, 1}};
            }
        }
    }
    return nullopt;
}

optional<array<int, 3>> find_collinear(const vector<Point3d>& points) {
    for (int u = 0, N = points.size(); u < N; u++) {
        for (int v = u + 1; v < N; v++) {
            for (int w = v + 1; w < N; w++) {
                if (collinear(points[u], points[v], points[w]))
                    return {{u, v, w}};
            }
        }
    }
    return nullopt;
}

optional<array<int, 2>> find_incident(const vector<Point3d>& points) {
    unordered_map<array<long, 3>, int> pointmap;
    for (int u = 0, N = points.size(); u < N; u++) {
        auto lattice = (1'000'000'000 * points[u]).round_lattice_point();
        if (pointmap.count(lattice))
            return {{u, pointmap[lattice]}};
        pointmap.emplace(lattice, u);
    }
    return nullopt;
}

} // namespace hull3d_utils

#endif // HULL3D_UTILS_HPP
