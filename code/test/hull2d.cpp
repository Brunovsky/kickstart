#include "test_utils.hpp"
#include "../geometry/hull2d.hpp"

using P = Point2d<long>;

inline namespace unit_testing {

void unit_test_convex_hull() {
    vector<P> a = {P(4, 1), P(6, 4), P(12, 5), P(9, 7), P(2, 6)};
    auto hull = graham_scan(a);
    assert(hull == vector<P>({P(4, 1), P(12, 5), P(9, 7), P(2, 6)}));

    a.push_back(P(8, 3));
    hull = graham_scan(a);
    assert(hull == vector<P>({P(4, 1), P(8, 3), P(12, 5), P(9, 7), P(2, 6)}));

    // gcc chokes on a.insert()
    vector<P> more = {P(9, 5), P(6, 5), P(3, 4), P(4, 6), P(10, 6), P(4, 2), P(7, 3)};
    a.insert(a.end(), begin(more), end(more));
    hull = graham_scan(a);
    assert(hull == vector<P>({P(4, 1), P(8, 3), P(12, 5), P(9, 7), P(2, 6)}));

    shuffle(begin(a), end(a), mt);
    hull = graham_scan(a);
    assert(hull == vector<P>({P(4, 1), P(8, 3), P(12, 5), P(9, 7), P(2, 6)}));

    shuffle(begin(a), end(a), mt);
    hull = graham_scan(a);
    assert(hull == vector<P>({P(4, 1), P(8, 3), P(12, 5), P(9, 7), P(2, 6)}));

    vector<P> b = {P(1, 1), P(2, 2), P(1, 2), P(3, 3), P(4, 4)};
    hull = graham_scan(b);
    assert(hull == vector<P>({P(1, 1), P(2, 2), P(3, 3), P(4, 4), P(1, 2)}));
}

} // namespace unit_testing

int main() {
    RUN_SHORT(unit_test_convex_hull());
    return 0;
}
