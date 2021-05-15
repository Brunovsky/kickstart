#ifndef HULL3D_UTILS_HPP
#define HULL3D_UTILS_HPP

#include "../hash.hpp"
#include "point3d.hpp" // Point3d, Plane

inline namespace hull3d_utils {

/**
 * Returns [v, f, kind] if there is a counterexample for this hull's correctness.
 * kind=0 ==> v is assigned to face f but isn't in its plane.
 * kind=1 ==> v can see face f (from outside).
 */
template <typename T, typename D = double>
optional<tuple<int, int, int>> verify_hull(const vector<vector<int>>& hull,
                                           const vector<Point3d<T, D>>& points) {
    int H = hull.size(), N = points.size();
    vector<Plane<T, D>> planes;
    vector<unordered_set<int>> vfaces(N);
    for (int f = 0; f < H; f++) {
        const auto& face = hull[f];
        const auto &u = points[face[0]], v = points[face[1]], w = points[face[2]];
        planes.emplace_back(u, v, w);
        for (int n : face) {
            vfaces[n].insert(f);
        }
    }
    for (int v = 0; v < N; v++) {
        for (int f : vfaces[v]) {
            if (planes[f].planeside(points[v]) != 0) {
                return {{v, f, 0}};
            }
        }
    }
    for (int v = 0; v < N; v++) {
        for (int f = 0; f < H; f++) {
            if (!vfaces[v].count(f) && planes[f].planeside(points[v]) == 1) {
                return {{v, f, 1}};
            }
        }
    }
    return nullopt;
}

template <typename T, typename D = double>
optional<array<int, 3>> find_collinear(const vector<Point3d<T, D>>& points) {
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

template <typename T, typename D = double>
optional<array<int, 2>> find_incident(const vector<Point3d<T, D>>& points) {
    unordered_map<Point3d<T, D>, int> pointmap;
    for (int u = 0, N = points.size(); u < N; u++) {
        auto [it, inserted] = pointmap.emplace(points[u], u);
        if (!inserted)
            return {{u, it->second}};
    }
    return nullopt;
}

} // namespace hull3d_utils

#endif // HULL3D_UTILS_HPP
