#ifndef HULL3D_UTILS_HPP
#define HULL3D_UTILS_HPP

#include "point3d.hpp" // Point3d, Plane

// *****

inline namespace hull3d_utils {

using hull_t = vector<vector<int>>;

/**
 * Rotate the faces in the hull so that the lowest index vertex is at the beginning.
 * Then sort all of the faces lexicographically.
 * You should simplify before canonicalizing.
 */
void canonicalize_hull(hull_t& faces) {
    for (auto& face : faces) {
        rotate(begin(face), min_element(begin(face), end(face)), end(face));
    }
    sort(begin(faces), end(faces));
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
template <typename P>
void simplify_hull(hull_t& faces, const vector<P>& points, int s = 0) {
    for (auto& face : faces) {
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

} // namespace hull3d_utils

#endif // HULL3D_UTILS_HPP
