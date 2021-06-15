#include "test_utils.hpp"
#include "../linear/matrix.hpp"
#include "../lib/anynum.hpp"
#include "../linear/linear_system.hpp"

#define F frac

inline namespace detail {

double min_diff(const vecd& u, const vecd& v) {
    double x = numeric_limits<double>::max();
    for (int i = 0, n = u.size(); i < n; i++) {
        x = min(x, abs(u[i] - v[i]));
    }
    return x;
}
double min_diff(const matd& u, const matd& v) {
    double x = numeric_limits<double>::max();
    for (int i = 0; i < u.n; i++) {
        x = min(x, min_diff(u[i], v[i]));
    }
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
        x[i] = uniform_gen<F>(minv, maxv, maxd);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = dista(mt);
    return make_pair(move(a), move(x));
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
    return make_pair(move(a), move(x));
}

} // namespace detail

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

void stress_test_gauss_frac() {
    intd distn(3, 7);
    int cnt_degenerate = 0, cnt_infeasible = 0, cnt_different = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, "stress test gauss frac");

        int n = distn(mt);
        auto [a, z] = generate_gauss_frac(n, -8, 8, 4);
        auto b = a * z;
        auto x = gauss(a, b);

        cnt_degenerate += !x;
        if (!x)
            continue;

        bool ok = a * *x == b;
        cnt_infeasible += !ok;
        cnt_different += ok && *x != z;
    }

    auto degenerate = format("{:5.1f}%", 100.0 * cnt_degenerate / runs);
    auto infeasible = format("{:5.1f}%", 100.0 * cnt_infeasible / runs);
    auto different = format("{:5.1f}%", 100.0 * cnt_different / runs);
    printcl(" {} degenerate  {} infeasible  {} different\n", degenerate, infeasible,
            different);
}

void stress_test_gauss_double() {
    intd distn(5, 100);
    int cnt_degenerate = 0, cnt_infeasible = 0, cnt_different = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, "stress test gauss double");

        int n = distn(mt);
        auto [a, z] = generate_gauss_double(n, -10, 10);
        auto b = a * z;
        auto x = gauss(a, b);

        cnt_degenerate += !x;
        if (!x)
            continue;

        bool ok = min_diff(a * *x, b) <= 1e-10;
        cnt_infeasible += !ok;
        cnt_different += ok && !(min_diff(*x, z) <= 1e-12);
    }

    auto degenerate = format("{:5.1f}%", 100.0 * cnt_degenerate / runs);
    auto infeasible = format("{:5.1f}%", 100.0 * cnt_infeasible / runs);
    auto different = format("{:5.1f}%", 100.0 * cnt_different / runs);
    printcl(" {} degenerate  {} infeasible  {} different\n", degenerate, infeasible,
            different);
}

void stress_test_inverse_double() {
    intd distn(5, 100);
    int cnt_degenerate = 0, cnt_diff_mul = 0, cnt_diff_inv = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (5s, now, runs) {
        print_time(now, 5s, "stress test inverse double");

        int n = distn(mt);
        auto [a, z] = generate_gauss_double(n, -10, 10);

        auto b = inverse(a);
        cnt_degenerate += !b;
        if (!b)
            continue;

        auto c = inverse(*b);
        cnt_degenerate += !c;
        if (!c)
            continue;

        cnt_diff_mul += !(min_diff(matd::identity(n), a * *b) <= 1e-16);
        cnt_diff_inv += !(min_diff(a, *c) <= 1e-12);
    }

    auto degenerate = format("{:5.1f}%", 100.0 * cnt_degenerate / runs);
    auto diff_mul = format("{:5.1f}%", 100.0 * cnt_diff_mul / runs);
    auto diff_inv = format("{:5.1f}%", 100.0 * cnt_diff_inv / runs);
    printcl(" {} degenerate  {} diff mul  {} diff inv\n", degenerate, diff_mul, diff_inv);
}

void scaling_test_gauss_double() {
    static vector<int> Ns = {5, 10, 20, 40, 70, 100, 150, 200, 300, 500};
    vector<vector<stringable>> table;
    table.push_back({"N", "degenerate", "infeasible", "different", "time"});

    for (int N : Ns) {
        string label = format("{0}x{0} scaling test gauss double", N);

        START_ACC(gauss);
        int run_degenerate = 0;
        int run_infeasible = 0;
        int run_different = 0;

        LOOP_FOR_DURATION_TRACKED_RUNS (800ms, now, runs) {
            print_time(now, 800ms, label);

            auto [a, z] = generate_gauss_double(N, -10, 10);
            auto b = a * z;

            START(gauss);
            auto x = gauss(a, b);
            ADD_TIME(gauss);

            run_degenerate += !x;
            if (!x)
                continue;

            bool ok = min_diff(a * *x, b) <= 1e-10;
            run_infeasible += !ok;
            run_different += ok && !(min_diff(*x, z) <= 1e-12);
        }

        auto degenerate = format("{:.1f}%", 100.0 * run_degenerate / runs);
        auto infeasible = format("{:.1f}%", 100.0 * run_infeasible / runs);
        auto different = format("{:.1f}%", 100.0 * run_different / runs);
        table.push_back({N, degenerate, infeasible, different, FORMAT_EACH(gauss, runs)});
    }

    print_time_table(table, "Scaling Gauss");
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
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 1},
        },
    }};
    vector<int> ans_det = {141267, 5870, 0, 1};
    vector<int> ans_rank = {5, 5, 4, 4};

    assert(quest.size() == ans_det.size());
    assert(quest.size() == ans_rank.size());
    int N = ans_det.size();

    for (int t = 0; t < N; t++) {
        int n = quest[t].size();
        matd a(n, n);
        matf b(n, n);
        matbf c(n, n);

        for (int i = 0; i < n; i++) {
            for (int j = 0, x; j < n; j++) {
                x = quest[t][i][j];
                a[i][j] = x;
                b[i][j] = x;
                c[i][j] = x;
            }
        }

        auto a_ans = det(a);
        auto b_ans = det(b);
        auto c_ans = det(c);
        int a_rank = matrank(a);
        int b_rank = matrank(b);
        int c_rank = matrank(c);

        assert(abs(a_ans - ans_det[t]) < 1e-10);
        assert(b_ans == ans_det[t]);
        assert(c_ans == ans_det[t]);
        assert(a_rank == ans_rank[t]);
        assert(b_rank == ans_rank[t]);
        assert(c_rank == ans_rank[t]);
    }
}

void unit_test_rank() {
    mat<double> a;

    a = mat<double>({
        {1, 3, 5},
        {2, 4, 6},
        {6, 7, 8},
        {1, 2, 3},
        {9, 8, 7},
    });
    print("rank(a) = {}\n", matrank(a));

    a = mat<double>({
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {1, 1, 1, 0},
    });
    print("rank(a) = {}\n", matrank(a));

    a = transpose(a);
    print("rank(transpose(a)) = {}\n", matrank(a));
}

int main() {
    RUN_SHORT(unit_test_gauss_frac());
    RUN_SHORT(unit_test_inverse_frac());
    RUN_SHORT(unit_test_det());
    RUN_SHORT(unit_test_rank());
    RUN_BLOCK(stress_test_gauss_frac());
    RUN_BLOCK(stress_test_gauss_double());
    RUN_BLOCK(stress_test_inverse_double());
    RUN_BLOCK(scaling_test_gauss_double());
    return 0;
}
