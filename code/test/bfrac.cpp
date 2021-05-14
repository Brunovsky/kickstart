#undef NDEBUG
#include "test_utils.hpp"
#include "../numeric/bfrac.hpp"

void unit_test_gcd() {
    assert(bfrac(93, 31) == bfrac(3, 1));
    assert(bfrac(7, -19) == bfrac(-7, 19));
    assert(bfrac(-74, -4) == bfrac(37, 2));
    assert(bfrac(3, 0) == bfrac(1, 0));
    assert(bfrac(-73, 0) == bfrac(-1, 0));
    assert(bfrac(-1, 0) < bigint("-1" + string(100, '0')));
    assert(bfrac(1, 0) > bigint("1" + string(100, '0')));
    assert(bfrac(7, -3) == bfrac(-7, 3));
    assert(floor(bfrac(7, 3)) == 2);
    assert(floor(bfrac(6, 3)) == 2);
    assert(floor(bfrac(-7, 3)) == -3);
    assert(floor(bfrac(-6, 3)) == -2);
    assert(ceil(bfrac(7, 3)) == 3);
    assert(ceil(bfrac(6, 3)) == 2);
    assert(ceil(bfrac(-7, 3)) == -2);
    assert(ceil(bfrac(-6, 3)) == -2);
}

void unit_test_ops() {
    assert(bfrac(2, 3) + bfrac(3, 4) == bfrac(17, 12));
    assert(abs(bfrac(2, 3) - bfrac(3, 4)) == bfrac(1, 12));
    assert(bfrac(3, 7) * bfrac(7, 8) == bfrac(3, 8));
    assert(bfrac(1, 2) / bfrac(1, 7) == bfrac(7, 2));
    assert(3 / bfrac(7, 2) == bfrac(6, 7));
    assert(4 * bfrac(7, 9) == bfrac(28, 9));
    assert(2 + bfrac(7, 9) == bfrac(25, 9));
    assert(1 - bfrac(7, 9) == bfrac(2, 9));
    assert(2 % bfrac(7, 9) == bfrac(4, 9));
    assert(bfrac(9, 2) / 3 == bfrac(3, 2));
    assert(bfrac(9, 2) / 4 == bfrac(9, 8));

    assert(bfrac(7, 3) % 2 == bfrac(1, 3));
    assert(bfrac(29, 9) % bfrac(6, 7) == bfrac(41, 63));
    assert(bfrac(29, 9) % bfrac(-6, 7) == bfrac(41, 63));
    assert(bfrac(-29, 9) % bfrac(6, 7) == bfrac(-41, 63));
    assert(bfrac(-29, 9) % bfrac(-6, 7) == bfrac(-41, 63));
}

// bfrac is stress tested by simplex

int main() {
    RUN_SHORT(unit_test_gcd());
    RUN_SHORT(unit_test_ops());
    return 0;
}
