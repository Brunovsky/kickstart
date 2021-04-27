#ifndef FRAC_HULL3D_FUNCTIONS_HPP
#define FRAC_HULL3D_FUNCTIONS_HPP

#include "frac_hull3d.hpp"

// *****

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

template <typename P>
double area_hull(const hull_t& hull, const vector<P>& points) {
    double hull_area = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_area += area(a, b, c);
        }
    }
    return hull_area;
}

template <typename P>
double volume_hull(const hull_t& hull, const vector<P>& points) {
    double hull_volume = 0;
    for (const auto& face : hull) {
        for (int i = 1, F = face.size(); i + 1 < F; i++) {
            const auto &a = points[face[0]], b = points[face[i]], c = points[face[i + 1]];
            hull_volume += double(volume(a, b, c, P::zero()));
        }
    }
    return hull_volume;
}

#endif // FRAC_HULL3D_FUNCTIONS_HPP
