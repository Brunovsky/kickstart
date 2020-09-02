#include "../matrix.hpp"

#include <fmt/format.h>

#include "../frac.hpp"
#include "../random.hpp"

using fmt::print;

// *****

using mat_t = mat<frac>;
using vec_t = vector<frac>;

void gauss_test() {
    mat_t m;
    vec_t sol;

    m = mat_t({
        {3, 2, -1},
        {2, -2, 4},
        {-1, frac(1, 2), -1},
    });
    sol = gauss(m, {1, -2, 0});
    assert(sol == vec_t({1, -2, -2}));

    m = mat_t({
        {1, 3, -2},
        {3, 5, 6},
        {2, 4, 3},
    });
    sol = gauss(m, {5, 7, 8});
    assert(sol == vec_t({-15, 8, 2}));
}

void inverse_test() {
    mat_t m, i, a;

    m = mat_t({
        {-1, frac(3, 2)},
        {1, -1},
    });
    i = mat_t({
        {2, 3},
        {2, 2},
    });
    a = inverse(m);
    assert(a == i);
    a = inverse(i);
    assert(a == m);

    m = mat_t({
        {2, -1, 0},
        {-1, 2, -1},
        {0, -1, 2},
    });
    i = mat_t({
        {frac(3, 4), frac(1, 2), frac(1, 4)},
        {frac(1, 2), 1, frac(1, 2)},
        {frac(1, 4), frac(1, 2), frac(3, 4)},
    });
    a = inverse(m);
    assert(a == i);
    a = inverse(i);
    assert(a == m);

    m = mat_t({
        {1, 2},
        {3, 4},
    });
    i = mat_t({
        {-2, 1},
        {frac(3, 2), frac(-1, 2)},
    });
    a = inverse(m);
    assert(a == i);
    a = inverse(i);
    assert(a == m);
}

void random_inverse_test(int R) {
    intd distn(5, 30);
    reald distv(-10, 10);

    for (int t = 1; t <= R; t++) {
        int n = distn(mt);
        mat<double> m(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                m[i][j] = distv(mt);
        auto a = inverse(inverse(m));

        bool ok = true;
        for (int i = 0; ok && i < n; i++)
            for (int j = 0; ok && j < n; j++)
                ok = abs(m[i][j] - a[i][j]) < 1e-5;
        if (!ok) {
            cout << to_string(m) << string(20, '=') << endl << to_string(a) << endl;
        }
        print("\rrandom inverse {}...", t);
        assert(ok);
    }
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    gauss_test();
    inverse_test();
    random_inverse_test(1000);
    return 0;
}
