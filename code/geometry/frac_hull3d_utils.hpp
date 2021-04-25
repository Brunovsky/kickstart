#ifndef FRAC_HULL3D_UTILS_HPP
#define FRAC_HULL3D_UTILS_HPP

#include "frac_point3d.hpp" // Point3d, Plane

// *****

inline namespace hull3d_utils {

using hull_t = vector<vector<int>>;

/**
 * Rotate the hull in the hull so that the lowest index vertex is at the beginning.
 * Then sort all of the hull lexicographically.
 * You should simplify before canonicalizing.
 */
void canonicalize_hull(hull_t& hull) {
    for (auto& face : hull) {
        rotate(begin(face), min_element(begin(face), end(face)), end(face));
    }
    sort(begin(hull), end(hull));
}

/**
 *  ,7,,,,,,
 *  ,,,,,,,,  The 2D convex hull of this plane is <7->5->6->9->4->2->1->0> but the points
 *  ,,,,,,,,  5, 0 and 1 are collinear with their neighbors, so the convex hull algorithm
 *  ,,,,0,,,  might "optimize" them away (i.e. it unfortunately won't find them).
 *  ,5,8,1,,  This is by design. If it finds them, it remembers them moving forward
 *  ,,,,,,2,  instead of detecting and discarding them. This means verification gets
 *  ,6,,3,,,  a bit tricky. To fix this issue we simply remove 5, 0 and 1 from the hull,
 *  ,,,,,4,,  and say that the simplified hull is
 *  ,,,9,,,,                      7->6->9->4->2
 * You should simplify before canonicalizing.
 */
template <typename F>
void simplify_hull(hull_t& hull, const vector<Point3d<F>>& points, int s = 0) {
    for (auto& face : hull) {
        vector<int> filtered_face;
        for (int j = 0, N = face.size(); j < N; j++) {
            int i = (j + N - 1) % N, k = (j + 1) % N;
            int u = face[i] - s, v = face[j] - s, w = face[k] - s;
            if (!collinear(points[u], points[v], points[w]))
                filtered_face.push_back(face[j]);
        }
        face = move(filtered_face);
    }
}

/**
 * Returns [v, f, kind] if there is a counterexample for this hull's correctness.
 * kind=0 ==> v is assigned to face f but isn't in its plane.
 * kind=1 ==> v can see face f (from outside).
 */
template <typename F>
optional<tuple<int, int, int>> verify_hull(const hull_t& hull,
                                           const vector<Point3d<F>>& points, int s = 0) {
    int H = hull.size(), N = points.size();
    vector<Plane<F>> planes;
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
