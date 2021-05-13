#ifndef FRAC_EXTRA_HPP
#define FRAC_EXTRA_HPP

#include "frac.hpp"

frac inv(frac f) { return frac(f.d, f.n); }

// fraction closest to f with denominator at most maxd
frac closest(frac f, long maxd) {
    long p0 = 0, q0 = 1;
    long p1 = 1, q1 = 0;
    long n = f.n, d = f.d;
    if (d <= maxd) {
        return f;
    }
    while (true) {
        long a = n / d;
        long p2 = p0 + a * p1;
        long q2 = q0 + a * q1;
        if (q2 > maxd) {
            break;
        }
        p0 = p1, q0 = q1;
        p1 = p2, q1 = q2;
        n = n - a * d;
        swap(n, d);
    }
    long k = (maxd - q0) / q1;
    frac b1(p0 + k * p1, q0 + k * q1);
    frac b2(p1, q1);
    return abs(b1 - f) <= abs(b2 - f) ? b1 : b2;
}

using cf_t = vector<long>;

// the first continued fraction representation of f
cf_t cf1_sequence(frac f) {
    cf_t cf1;
    cf1.push_back(floor(f));
    f = f - floor(f);
    while (f.n) {
        swap(f.n, f.d);
        cf1.push_back(f.n / f.d);
        f.n = f.n % f.d;
    }
    return cf1;
}

// the second continued fraction representation of f:  a] --> a-1,1]
cf_t cf2_sequence(frac f) {
    cf_t cf2 = cf1_sequence(f);
    if (!cf2.empty()) {
        cf2.back()--;
        cf2.push_back(1);
    }
    return cf2;
}

// compute continued fraction representation
frac compute_fraction(const cf_t& cf) {
    frac f(0, 1);
    for (int i = cf.size() - 1; i >= 0; i--) {
        f.n += cf[i] * f.d;
        swap(f.n, f.d);
    }
    return inv(f);
}

// from codejam: compute fraction between lo and hi with smallest possible denominator
frac cf_search(frac lo, frac hi) {
    assert(lo < hi);
    long a = floor(lo);
    if (hi > a + 1) {
        return a + 1;
    }
    frac fa = frac(a);
    if (lo.d == 1) {
        return fa + inv(inv(hi - fa) + 1);
    } else {
        return fa + inv(cf_search(inv(hi - fa), inv(lo - fa)));
    }
}

#endif // FRAC_EXTRA_HPP
