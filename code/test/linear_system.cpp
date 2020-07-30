#include "../linear_system.hpp"

using namespace std;

// *****

void test_gauss() {
    linear_system ls;
    linear_system::Result res;
    ls.A = {
        {3, 2, -1},
        {2, -2, 4},
        {-1, frac(1, 2), -1},
    };
    ls.b = {1, -2, 0};
    res = ls.compute();
    assert(res == linear_system::SOLVED);
    assert(ls.x == vector<frac>({1, -2, -2}));

    ls.A = {
        {1, 3, -2},
        {3, 5, 6},
        {2, 4, 3},
    };
    ls.b = {5, 7, 8};
    res = ls.compute();
    assert(res == linear_system::SOLVED);
    assert(ls.x == vector<frac>({-15, 8, 2}));
}

void test_inverse() {
    inverse inv;
    bool invertible;
    inv.A = {
        {-1, frac(3, 2)},
        {1, -1},
    };
    invertible = inv.compute();
    assert(invertible);
    assert(inv.B == vector<vector<frac>>({{2, 3}, {2, 2}}));

    inv.A = {
        {2, -1, 0},
        {-1, 2, -1},
        {0, -1, 2},
    };
    invertible = inv.compute();
    assert(invertible);
    assert(inv.B == vector<vector<frac>>({{frac(3, 4), frac(1, 2), frac(1, 4)},
                                          {frac(1, 2), 1, frac(1, 2)},
                                          {frac(1, 4), frac(1, 2), frac(3, 4)}}));

    inv.A = {
        {1, 2},
        {3, 4},
    };
    invertible = inv.compute();
    assert(invertible);
    assert(inv.B == vector<vector<frac>>({{-2, 1}, {frac(3, 2), frac(-1, 2)}}));

    inv.A = {
        {6, 8},
        {3, 4},
    };
    invertible = inv.compute();
    assert(!invertible);
}

int main() {
    test_gauss();
    test_inverse();
    return 0;
}
