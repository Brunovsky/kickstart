#include "../frac.hpp"

// *****

int main() {
    frac pi(3141592653, 1000000000);
    assert(closest(pi, 6) == frac(19, 6));
    assert(closest(pi, 7) == frac(22, 7));
    assert(closest(pi, 30) == frac(22, 7));
    assert(closest(pi, 100) == frac(311, 99));
    assert(closest(pi, 90000) == frac(103993, 33102));
    assert(frac(93, 31) == frac(3, 1));
    assert(frac(7, -19) == frac(-7, 19));
    assert(frac(-74, -4) == frac(37, 2));
    assert(frac(2, 3) + frac(3, 4) == frac(17, 12));
    assert(absdif(frac(2, 3), frac(3, 4)) == frac(1, 12));
    return 0;
}
