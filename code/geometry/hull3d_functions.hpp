#pragma once

#include "hull3d.hpp"

/**
 * Rotate the faces in the hull so that the lowest index vertex is at the beginning.
 * Then sort all of the faces lexicographically.
 */
void canonicalize_hull(vector<vector<int>>& hull) {
    for (auto& face : hull) {
        rotate(begin(face), min_element(begin(face), end(face)), end(face));
    }
    sort(begin(hull), end(hull));
}

template <typename T, typename D = double>
D area_hull(const vector<vector<int>>& hull, const vector<Point3d<T, D>>& points) {
    D hull_area = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_area += a.crossed(b, c).norm();
        }
    }
    return hull_area / 2.0;
}

template <typename T, typename D = double>
D volume_hull(const vector<vector<int>>& hull, const vector<Point3d<T, D>>& points) {
    D hull_volume = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_volume += D(dot(a, cross(b, c)));
        }
    }
    return hull_volume / 6.0;
}
