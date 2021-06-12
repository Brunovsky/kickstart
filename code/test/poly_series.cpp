#include "test_utils.hpp"
#include "../numeric/fft.hpp"
#include "../numeric/poly.hpp"
#include "../numeric/poly_multieval.hpp"
#include "../numeric/poly_series.hpp"
#include "../lib/anynum.hpp"

// Assumes FFT is not enabled in poly.hpp

using namespace polymath;
using num = modnum<998244353>;

int stir_1st[10][10] = {
    {1},
    {0, 1},
    {0, 1, 1},
    {0, 2, 3, 1},
    {0, 6, 11, 6, 1},
    {0, 24, 50, 35, 10, 1},
    {0, 120, 274, 225, 85, 15, 1},
    {0, 720, 1764, 1624, 735, 175, 21, 1},
    {0, 5040, 13068, 13132, 6769, 1960, 322, 28, 1},
    {0, 40320, 109584, 118124, 67284, 22449, 4536, 546, 36, 1},
};
int stir_2nd[10][10] = {
    {1},
    {0, 1},
    {0, 1, 1},
    {0, 1, 3, 1},
    {0, 1, 7, 6, 1},
    {0, 1, 15, 25, 10, 1},
    {0, 1, 31, 90, 65, 15, 1},
    {0, 1, 63, 301, 350, 140, 21, 1},
    {0, 1, 127, 966, 1701, 1050, 266, 28, 1},
    {0, 1, 255, 3025, 7770, 6951, 2646, 462, 36, 1},
};

void unit_test_polyseries() {
    print("faul(20, 5) = {}\n", faulhaber<num>(20, 5));
    print("faul(30, 5) = {}\n", faulhaber<num>(30, 5));
    print("falling(10) = {}\n", falling_factorial<long>(10));
    print("falling(12) = {}\n", falling_factorial<long>(12));
    print("rising(10) = {}\n", rising_factorial<long>(10));
    print("rising(12) = {}\n", rising_factorial<long>(12));
    for (int i = 0; i <= 9; i++) {
        int n = i + 1;
        auto s1 = stirling_1st<int>(i);
        print("stirling_1st({}) = {}\n", i, s1);
        assert(equal(s1.data(), s1.data() + n, stir_1st[i], stir_1st[i] + n));
    }
    for (int i = 0; i <= 9; i++) {
        int n = i + 1;
        auto s2 = stirling_2nd<num>(i);
        print("stirling_2nd({}) = {}\n", i, s2);
        assert(equal(s2.data(), s2.data() + n, stir_2nd[i], stir_2nd[i] + n));
    }
}

int main() {
    RUN_SHORT(unit_test_polyseries());
    return 0;
}
