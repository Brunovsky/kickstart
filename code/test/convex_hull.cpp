#include "../convex_hull.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

// *****

void unit_test_convex_hull() {
    vector<point2d> a = {{4, 1}, {6, 4}, {12, 5}, {9, 7}, {2, 6}};
    auto hull = graham_scan(a);
    assert(hull == vector<point2d>({{4, 1}, {12, 5}, {9, 7}, {2, 6}}));

    a.push_back({8, 3});
    hull = graham_scan(a);
    assert(hull == vector<point2d>({{4, 1}, {8, 3}, {12, 5}, {9, 7}, {2, 6}}));

    // gcc chokes on a.insert()
    vector<point2d> more = {{9, 5}, {6, 5}, {3, 4}, {4, 6}, {10, 6}, {4, 2}, {7, 3}};
    a.insert(a.end(), begin(more), end(more));
    hull = graham_scan(a);
    assert(hull == vector<point2d>({{4, 1}, {8, 3}, {12, 5}, {9, 7}, {2, 6}}));

    shuffle(begin(a), end(a), mt);
    hull = graham_scan(a);
    assert(hull == vector<point2d>({{4, 1}, {8, 3}, {12, 5}, {9, 7}, {2, 6}}));

    shuffle(begin(a), end(a), mt);
    hull = graham_scan(a);
    assert(hull == vector<point2d>({{4, 1}, {8, 3}, {12, 5}, {9, 7}, {2, 6}}));

    vector<point2d> b = {{1, 1}, {2, 2}, {1, 2}, {3, 3}, {4, 4}};
    hull = graham_scan(b);
    assert(hull == vector<point2d>({{1, 1}, {2, 2}, {3, 3}, {4, 4}, {1, 2}}));
}

int main() {
    unit_test_convex_hull();
    return 0;
}
