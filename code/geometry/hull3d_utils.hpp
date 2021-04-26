#ifndef HULL3D_UTILS_HPP
#define HULL3D_UTILS_HPP

#include "hash.hpp"
#include "point3d.hpp" // Point3d, Plane

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
void simplify_hull(hull_t& hull, const vector<Point3d>& points, int s, double eps) {
    for (auto& face : hull) {
        vector<int> filtered_face;
        for (int j = 0, N = face.size(); j < N; j++) {
            int i = (j + N - 1) % N, k = (j + 1) % N;
            int u = face[i] - s, v = face[j] - s, w = face[k] - s;
            if (!collinear(points[u], points[v], points[w], eps))
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
