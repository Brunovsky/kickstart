#ifndef CONVEX_HULL_HPP
#define CONVEX_HULL_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

using point2d = array<int, 2>;

long cross_product(const point2d& p0, const point2d& p1, const point2d& p2) {
    long x0 = p0[0], y0 = p0[1];
    long x1 = p1[0], y1 = p1[1];
    long x2 = p2[0], y2 = p2[1];
    return (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
}

vector<point2d> graham_scan(vector<point2d> points) {
    if (points.size() <= 2UL)
        return points;

    point2d lo = points[0];
    for (const point2d& point : points) {
        if (point[1] < lo[1] || (point[1] == lo[1] && point[0] < lo[0])) {
            lo = point;
        }
    }

    sort(begin(points), end(points), [&](const point2d& lhs, const point2d& rhs) {
        auto cross = cross_product(lo, lhs, rhs);
        return cross == 0 ? lhs[1] < rhs[1] : cross > 0;
    });

    vector<point2d> hull{points[0], points[1]};
    int s = 1, n = points.size();

    for (int i = 2; i < n; i++) {
        while (cross_product(hull[s - 1], hull[s], points[i]) < 0) {
            hull.pop_back(), s--;
            assert(s >= 1);
        }
        hull.push_back(points[i]), s++;
    }

    return hull;
}

#endif // CONVEX_HULL_HPP
