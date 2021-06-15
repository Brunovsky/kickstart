#include "test_utils.hpp"
#include "../numeric/frac.hpp"
#include "../numeric/frac_extra.hpp"

void unit_test_gcd() {
    assert(frac(93, 31) == frac(3, 1));
    assert(frac(7, -19) == frac(-7, 19));
    assert(frac(-74, -4) == frac(37, 2));
    assert(frac(3, 0) == frac(1, 0));
    assert(frac(-73, 0) == frac(-1, 0));
    assert(frac(-1, 0) < LONG_MIN);
    assert(frac(1, 0) > LONG_MAX);
    assert(frac(7, -3) == frac(-7, 3));
    assert(floor(frac(7, 3)) == 2L);
    assert(floor(frac(6, 3)) == 2L);
    assert(floor(frac(-7, 3)) == -3L);
    assert(floor(frac(-6, 3)) == -2L);
    assert(ceil(frac(7, 3)) == 3L);
    assert(ceil(frac(6, 3)) == 2L);
    assert(ceil(frac(-7, 3)) == -2L);
    assert(ceil(frac(-6, 3)) == -2L);
}

void unit_test_ops() {
    assert(frac(2, 3) + frac(3, 4) == frac(17, 12));
    assert(abs(frac(2, 3) - frac(3, 4)) == frac(1, 12));
    assert(frac(3, 7) * frac(7, 8) == frac(3, 8));
    assert(frac(1, 2) / frac(1, 7) == frac(7, 2));
    assert(3L / frac(7, 2) == frac(6, 7));
    assert(4L * frac(7, 9) == frac(28, 9));
    assert(2L + frac(7, 9) == frac(25, 9));
    assert(1L - frac(7, 9) == frac(2, 9));
    assert(2L % frac(7, 9) == frac(4, 9));
    assert(frac(9, 2) / 3L == frac(3, 2));
    assert(frac(9, 2) / 4L == frac(9, 8));

    assert(frac(7, 3) % 2L == frac(1, 3));
    assert(frac(29, 9) % frac(6, 7) == frac(41, 63));
    assert(frac(29, 9) % frac(-6, 7) == frac(41, 63));
    assert(frac(-29, 9) % frac(6, 7) == frac(-41, 63));
    assert(frac(-29, 9) % frac(-6, 7) == frac(-41, 63));
}

void unit_test_closest() {
    frac pi(3141592653, 1000000000);
    assert(closest(pi, 6) == frac(19, 6));
    assert(closest(pi, 7) == frac(22, 7));
    assert(closest(pi, 30) == frac(22, 7));
    assert(closest(pi, 100) == frac(311, 99));
    assert(closest(pi, 90000) == frac(103993, 33102));
}

void unit_test_continued_fractions() {
    auto cf = cf1_sequence(frac(649, 200));
    assert(cf == cf_t({3, 4, 12, 4}));
    auto cf2 = cf2_sequence(frac(649, 200));
    assert(cf2 == cf_t({3, 4, 12, 3, 1}));
    assert(compute_fraction(cf) == frac(649, 200));
    assert(compute_fraction(cf2) == frac(649, 200));
    cf = cf1_sequence(frac(7, 1));
    assert(cf == cf_t({7}));
    assert(compute_fraction(cf) == frac(7, 1));
    cf = cf1_sequence(frac(1, 93));
    assert(cf == cf_t({0, 93}));
    assert(compute_fraction(cf) == frac(1, 93));
}

void unit_test_read() {
    assert(stofrac("-123") == -123);
    assert(stofrac("-123.00") == -123);
    assert(stofrac("-123.5") == frac(-247, 2));
    assert(stofrac(" -247/2") == frac(-247, 2));
    assert(stofrac("  -494/4") == frac(-247, 2));
    assert(stofrac(" -494/-4") == -494);
    assert(stofrac("+7/8") == frac(7, 8));
    assert(stofrac(" 123/456") == frac(123, 456));
    assert(stofrac(" 123/0") == frac(1, 0));
    assert(stofrac(" 0/123 ") == 0);
    assert(stofrac("  1.5") == frac(3, 2));
    assert(stofrac(" 1.") == 1);
    assert(stofrac(" .1") == frac(1, 10));
    assert(stofrac(" -.100") == frac(-1, 10));
    assert(stofrac(" -712412.71231") == -frac(71241271231, 100000));
    assert(stofrac(" -712412/71231") == -frac(712412, 71231));
}

// frac is stress tested by simplex

int main() {
    RUN_SHORT(unit_test_gcd());
    RUN_SHORT(unit_test_ops());
    RUN_SHORT(unit_test_closest());
    RUN_SHORT(unit_test_continued_fractions());
    RUN_SHORT(unit_test_read());
    return 0;
}
