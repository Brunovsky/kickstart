#ifndef GENERATORS_FRAC_HPP
#define GENERATORS_FRAC_HPP

#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"
#include "../random.hpp"

auto gen_frac(long minv, long maxv, long maxd) {
    long d = longd(1, maxd)(mt);
    longd dist(minv * d, maxv * d);
    return frac(dist(mt), d);
}

auto gen_bfrac(int minv, int maxv, int maxd) {
    int d = intd(1, maxd)(mt);
    intd dist(minv * d, maxv * d);
    return bfrac(dist(mt), d);
}

#endif // GENERATORS_FRAC_HPP
