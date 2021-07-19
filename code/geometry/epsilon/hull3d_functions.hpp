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

double area_hull(const vector<vector<int>>& hull, const vector<Point3d>& points) {
    double hull_area = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_area += area(a, b, c);
        }
    }
    return hull_area;
}

double volume_hull(const vector<vector<int>>& hull, const vector<Point3d>& points) {
    double hull_volume = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_volume += volume(a, b, c, Point3d());
        }
    }
    return hull_volume;
}
