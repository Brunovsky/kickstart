#include "test_utils.hpp"
#include "../linear/matrix.hpp"
#include "../linear/linear_system.hpp"

#define F frac

inline namespace {

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

double min_diff(const vecd& u, const vecd& v) {
    double x = numeric_limits<double>::max();
    for (int i = 0, n = u.size(); i < n; i++)
        x = min(x, abs(u[i] - v[i]));
    return x;
}
double min_diff(const matd& u, const matd& v) {
    double x = numeric_limits<double>::max();
    for (int i = 0; i < u.n; i++)
        x = min(x, min_diff(u[i], v[i]));
    return x;
}

template <typename T>
bool feasible(const mat<T>& a, const vector<T>& b, const vector<T>& x) {
    auto z = a * x - b;
    return all_of(begin(z), end(z), [](T c) { return mat_zero(c); });
}

auto generate_gauss_frac(int n, long minv, long maxv, long maxd) {
    matf a(n, n);
    vecf x(n);
    longd dista(minv, maxv);
    for (int i = 0; i < n; i++)
        x[i] = gen_frac(minv, maxv, maxd);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = dista(mt);
    return tuple<matf, vecf>{move(a), move(x)};
}

auto generate_gauss_double(int n, double minv, double maxv) {
    matd a(n, n);
    vecd x(n);
    reald dista(minv, maxv);
    for (int i = 0; i < n; i++)
        x[i] = dista(mt);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = dista(mt);
    return tuple<matd, vecd>{move(a), move(x)};
}

} // namespace

void unit_test_gauss_frac() {
    matf m;
    vecf b, x;

    m = matf({
        {3, 2, -1},
        {2, -2, 4},
        {-1, F(1, 2), -1},
    });
    b = {1, -2, 0};
    x = *gauss(m, b);
    assert(x == vecf({1, -2, -2}));
    assert(m * x == b);

    m = matf({
        {1, 3, -2},
        {3, 5, 6},
        {2, 4, 3},
    });
    b = {5, 7, 8};
    x = *gauss(m, b);
    assert(x == vecf({-15, 8, 2}));
    assert(m * x == b);
}

void unit_test_inverse_frac() {
    matf m, i, a;

    m = matf({
        {-1, F(3, 2)},
        {1, -1},
    });
    i = matf({
        {2, 3},
        {2, 2},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);

    m = matf({
        {2, -1, 0},
        {-1, 2, -1},
        {0, -1, 2},
    });
    i = matf({
        {F(3, 4), F(1, 2), F(1, 4)},
        {F(1, 2), 1, F(1, 2)},
        {F(1, 4), F(1, 2), F(3, 4)},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);

    m = matf({
        {1, 2},
        {3, 4},
    });
    i = matf({
        {-2, 1},
        {F(3, 2), F(-1, 2)},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);
}

void stress_test_gauss_frac(int T = 2000) {
    intd distn(3, 7);
    int degenerate = 0, infeasible = 0, different = 0;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test gauss frac");
        int n = distn(mt);
        auto [a, z] = generate_gauss_frac(n, -8, 8, 4);
        auto b = a * z;
        auto x = gauss(a, b);
        degenerate += !x;
        if (!x)
            continue;
        bool ok = a * *x == b;
        infeasible += !ok;
        different += ok && *x != z;
    }
    print("  {:5.1f}% ({}) degenerate\n", 100.0 * degenerate / T, degenerate);
    print("  {:5.1f}% ({}) infeasible\n", 100.0 * infeasible / T, infeasible);
    print("  {:5.1f}% ({}) different\n", 100.0 * different / T, different);
}

void stress_test_gauss_double(int T = 1000) {
    intd distn(5, 100);
    int degenerate = 0, infeasible = 0, different = 0;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test gauss double");
        int n = distn(mt);
        auto [a, z] = generate_gauss_double(n, -10, 10);
        auto b = a * z;
        auto x = gauss(a, b);
        degenerate += !x;
        if (!x)
            continue;
        bool ok = min_diff(a * *x, b) <= 1e-10;
        infeasible += !ok;
        different += ok && !(min_diff(*x, z) <= 1e-12);
    }
    print("  {:5.1f}% ({}) degenerate\n", 100.0 * degenerate / T, degenerate);
    print("  {:5.1f}% ({}) infeasible\n", 100.0 * infeasible / T, infeasible);
    print("  {:5.1f}% ({}) different\n", 100.0 * different / T, different);
}

void stress_test_inverse_double(int T = 1000) {
    intd distn(5, 100);
    int degenerate = 0, different_mul = 0, different_inv = 0;

    for (int t = 0; t <= T; t++) {
        print_progress(t, T, "stress test inverse double");
        int n = distn(mt);
        auto [a, z] = generate_gauss_double(n, -10, 10);

        auto b = inverse(a);
        degenerate += !b;
        if (!b)
            continue;
        auto c = inverse(*b);
        degenerate += !c;
        if (!c)
            continue;
        different_mul += !(min_diff(matd::identity(n), a * *b) <= 1e-16);
        different_inv += !(min_diff(a, *c) <= 1e-12);
    }
    print("  {:5.1f}% ({}) degenerate\n", 100.0 * degenerate / T, degenerate);
    print("  {:5.1f}% ({}) different mul\n", 100.0 * different_mul / T, different_mul);
    print("  {:5.1f}% ({}) different inv\n", 100.0 * different_inv / T, different_inv);
}

void scaling_test_gauss_double(int T = 100, int nmin = 5, int nmax = 300, int ninc = 5) {
    int total_degenerate = 0, total_infeasible = 0, total_different = 0;

    for (int n = nmin; n <= nmax; n += ninc) {
        string label = format("{} stress test gauss double", n);
        START_ACC(gauss);
        int degenerate = 0, infeasible = 0, different = 0;

        for (int t = 0; t < T; t++) {
            print_progress(t, T, label);
            auto [a, z] = generate_gauss_double(n, -10, 10);
            auto b = a * z;

            START(gauss);
            auto x = gauss(a, b);
            ADD_TIME(gauss);

            degenerate += !x;
            if (!x)
                continue;
            bool ok = min_diff(a * *x, b) <= 1e-10;
            infeasible += !ok;
            different += ok && !(min_diff(*x, z) <= 1e-12);
        }

        clear_line();
        print(" {:>8}ms -- {:>3} -- {:5.1f}%  {:5.1f}%  {:5.1f}%\n", TIME_MS(gauss), n,
              100.0 * degenerate / T, 100.0 * infeasible / T, 100.0 * different / T);
        total_degenerate += degenerate;
        total_infeasible += infeasible;
        total_different += different;
    }

    int N = T * (nmax - nmin + ninc) / ninc;
    print("  {:5.1f}% ({}) degenerate\n", 100.0 * total_degenerate / N, total_degenerate);
    print("  {:5.1f}% ({}) infeasible\n", 100.0 * total_infeasible / N, total_infeasible);
    print("  {:5.1f}% ({}) different\n", 100.0 * total_different / N, total_different);
}

void unit_test_det() {
    vector<vector<vector<int>>> quest = {{
        {
            {2, 7, 9, 3, 4},
            {-6, 11, 2, -4, 0},
            {0, -2, 8, -9, 6},
            {5, 10, -1, -3, -7},
            {1, -8, -5, -10, -12},
        },
        {
            {3, 4, -7, 6, -9},
            {-4, -2, 8, 1, 3},
            {5, -1, 0, 7, 6},
            {-2, 6, -2, -8, 2},
            {1, 1, 1, 1, 1},
        },
        {
            {3, 4, -7, 6, -9},
            {-4, -2, 8, 1, 3},
            {5, -1, 0, 7, 6},
            {-2, 6, -2, -8, 2},
            {8, 3, 7, 21, 17},
        },
    }};
    vector<int> ans = {141267, 5870, 0};

    assert(quest[0][2][3] == -9 && quest.size() == ans.size());

    for (int t = 0, N = ans.size(); t < N; t++) {
        int n = quest[t].size();
        matd a(n, n);
        matf b(n, n);
        matbf c(n, n);

        for (int i = 0; i < n; i++)
            for (int j = 0, x; j < n; j++)
                x = quest[t][i][j], a[i][j] = x, b[i][j] = x, c[i][j] = x;

        double a_ans = det(a);
        frac b_ans = det(b);
        bfrac c_ans = det(c);

        assert(abs(a_ans - ans[t]) < 1e-10);
        assert(b_ans == ans[t]);
        assert(c_ans == ans[t]);
    }
}

int main() {
    RUN_SHORT(unit_test_gauss_frac());
    RUN_SHORT(unit_test_inverse_frac());
    RUN_SHORT(unit_test_det());
    RUN_BLOCK(stress_test_gauss_frac());
    RUN_BLOCK(stress_test_gauss_double());
    RUN_BLOCK(stress_test_inverse_double());
    RUN_BLOCK(scaling_test_gauss_double());
    return 0;
}
