#ifndef HULL2D_HPP
#define HULL2D_HPP

#include "point2d.hpp" // Point2d

template <typename P>
vector<P> graham_scan(vector<P> points) {
    int N = points.size();
    if (N <= 2) {
        return points;
    }

    auto lo = points[0];
    for (const auto& point : points) {
        if (make_pair(point[1], point[0]) < make_pair(lo[1], lo[0])) {
            lo = point;
        }
    }

    sort(begin(points), end(points), [&](const P& lhs, const P& rhs) {
        auto cross = lo.crossed(lhs, rhs);
        return make_pair(cross, rhs[1]) > make_pair(decltype(cross)(0), lhs[1]);
    });

    int s = 0;
    vector<P> hull{points[0], points[1]};
    for (int i = 2; i < N; i++) {
        while (hull[s].crossed(hull[s + 1], points[i]) < 0) {
            hull.pop_back(), s--;
            assert(s >= 0);
        }
        hull.push_back(points[i]), s++;
    }
    return hull;
}

#endif // HULL2D_HPP
