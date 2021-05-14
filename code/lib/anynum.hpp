#ifndef LIB_ANY_NUM
#define LIB_ANY_NUM

#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"
#include "../random.hpp"

template <typename T>
auto uniform_gen(long minv, long maxv, long maxd) {
    assert(max(abs(minv), abs(maxv)) <= LONG_MAX / maxd);
    if constexpr (is_same_v<T, frac>) {
        long d = longd(1, maxd)(mt);
        longd dist(minv * d, maxv * d);
        return frac(dist(mt), d);
    } else if constexpr (is_same_v<T, bfrac>) {
        int d = intd(1, maxd)(mt);
        intd dist(minv * d, maxv * d);
        return bfrac(dist(mt), d);
    } else if constexpr (is_integral_v<T>) {
        return longd(minv, maxv)(mt);
    } else if constexpr (is_arithmetic_v<T>) {
        long n = longd(minv * maxd, maxv * maxd)(mt);
        return T(n) / T(maxd);
    }
}

#endif // LIB_ANY_NUM
