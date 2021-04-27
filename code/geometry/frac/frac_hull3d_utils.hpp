#ifndef FRAC_HULL3D_UTILS_HPP
#define FRAC_HULL3D_UTILS_HPP

#include "frac_point3d.hpp" // Point3d, Plane

// *****

inline namespace hull3d_utils {

/**
 * Returns [v, f, kind] if there is a counterexample for this hull's correctness.
 * kind=0 ==> v is assigned to face f but isn't in its plane.
 * kind=1 ==> v can see face f (from outside).
 */
template <typename F>
optional<tuple<int, int, int>> verify_hull(const hull_t& hull,
                                           const vector<Point3d<F>>& points,
                                           int skip_0 = 0) {
    int H = hull.size(), N = points.size();
    vector<Plane<F>> planes;
    vector<unordered_set<int>> vfaces(N);
    for (int f = 0; f < H; f++) {
        const auto& face = hull[f];
        int u = face[0], v = face[1], w = face[2];
        planes.emplace_back(points[u], points[v], points[w]);
        for (int v : face) {
            vfaces[v].insert(f);
        }
    }
    for (int v = skip_0; v < N; v++) {
        for (int f : vfaces[v]) {
            if (planes[f].planeside(points[v]) != 0) {
                return {{v, f, 0}};
            }
        }
    }
    for (int v = skip_0; v < N; v++) {
        for (int f = 0; f < H; f++) {
            if (!vfaces[v].count(f) && planes[f].planeside(points[v]) == 1) {
                return {{v, f, 1}};
            }
        }
    }
    return nullopt;
}

template <typename F>
optional<array<int, 4>> find_coplanar(const vector<Point3d<F>>& points) {
    unordered_map<Plane<F>, array<int, 3>, PlaneHasher> planes;
    for (int u = 0, N = points.size(); u < N; u++) {
        for (int v = u + 1; v < N; v++) {
            for (int w = v + 1; w < N; w++) {
                Plane<F> plane(points[u], points[v], points[w]);
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
    return nullopt;
}

template <typename F>
optional<array<int, 3>> find_collinear(const vector<Point3d<F>>& points) {
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

template <typename F>
optional<array<int, 2>> find_incident(const vector<Point3d<F>>& points) {
    unordered_map<Point3d<F>, int, Hasher3d> pointmap;
    for (int u = 0, N = points.size(); u < N; u++) {
        if (pointmap.count(points[u]))
            return {{u, pointmap[points[u]]}};
        pointmap.emplace(points[u], u);
    }
    return nullopt;
}

} // namespace hull3d_utils

#endif // FRAC_HULL3D_UTILS_HPP
