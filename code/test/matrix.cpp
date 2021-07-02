#include "test_utils.hpp"
#include "../linear/matrix.hpp"
#include "../numeric/frac.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/modnum.hpp"
#include "../lib/anynum.hpp"

constexpr int MOD = 998244353;
#define F frac
using matf = mat<frac>;
using matd = mat<double>;
using matn = mat<modnum<MOD>>;
using vecf = vector<frac>;
using vecd = vector<double>;
using vecn = vector<modnum<MOD>>;

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
    for (int i = 0; i < u.rows(); i++) {
        for (int j = 0; j < u.cols(); j++) {
            x = min(x, abs(u[{i, j}] - v[{i, j}]));
        }
    }
    return x;
}

auto generate_gauss_modnum(int n) {
    matn a({n, n});
    vecn x(n);
    intd dista(0, MOD - 1);
    for (int i = 0; i < n; i++)
        x[i] = dista(mt);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[{i, j}] = dista(mt);
    return make_pair(move(a), move(x));
}

auto generate_gauss_double(int n, double minv, double maxv) {
    matd a({n, n});
    vecd x(n);
    reald dista(minv, maxv);
    for (int i = 0; i < n; i++)
        x[i] = dista(mt);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[{i, j}] = dista(mt);
    return make_pair(move(a), move(x));
}

} // namespace detail

void unit_test_gauss_frac() {
    matf m;
    vecf b, x;

    m = matf::from({
        {3, 2, -1},
        {2, -2, 4},
        {-1, F(1, 2), -1},
    });
    b = {1, -2, 0};
    x = *solve_linear_system(m, b);
    assert(x == vecf({1, -2, -2}));
    assert(m * x == b);

    m = matf::from({
        {1, 3, -2},
        {3, 5, 6},
        {2, 4, 3},
    });
    b = {5, 7, 8};
    x = *solve_linear_system(m, b);
    assert(x == vecf({-15, 8, 2}));
    assert(m * x == b);
}

void unit_test_inverse_frac() {
    matf m, i, a;

    m = matf::from({
        {-1, F(3, 2)},
        {1, -1},
    });
    i = matf::from({
        {2, 3},
        {2, 2},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);

    m = matf::from({
        {2, -1, 0},
        {-1, 2, -1},
        {0, -1, 2},
    });
    i = matf::from({
        {F(3, 4), F(1, 2), F(1, 4)},
        {F(1, 2), 1, F(1, 2)},
        {F(1, 4), F(1, 2), F(3, 4)},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);

    m = matf::from({
        {1, 2},
        {3, 4},
    });
    i = matf::from({
        {-2, 1},
        {F(3, 2), F(-1, 2)},
    });
    a = *inverse(m);
    assert(a == i);
    a = *inverse(i);
    assert(a == m);
}

void stress_test_gauss_modnum() {
    intd distn(20, 50);
    int cnt_degenerate = 0, cnt_infeasible = 0, cnt_different = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, "stress test gauss modnum");

        int n = distn(mt);
        auto [a, z] = generate_gauss_modnum(n);
        auto b = a * z;
        auto x = solve_linear_system(a, b);

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
    printcl(" {} degenerate | bad: {} infeasible  {} different\n", degenerate, infeasible,
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
        auto x = solve_float_linear_system(a, b);

        cnt_degenerate += !x;
        if (!x)
            continue;

        bool ok = min_diff(a * *x, b) <= 1e-8;
        cnt_infeasible += !ok;
        cnt_different += ok && !(min_diff(*x, z) <= 1e-10);
    }

    auto degenerate = format("{:5.1f}%", 100.0 * cnt_degenerate / runs);
    auto infeasible = format("{:5.1f}%", 100.0 * cnt_infeasible / runs);
    auto different = format("{:5.1f}%", 100.0 * cnt_different / runs);
    printcl(" {} degenerate | bad: {} infeasible  {} different\n", degenerate, infeasible,
            different);
}

void stress_test_inverse_double() {
    intd distn(5, 100);
    int cnt_degen1 = 0, cnt_degen2 = 0, cnt_diff_mul = 0, cnt_diff_inv = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (5s, now, runs) {
        print_time(now, 5s, "stress test inverse double");

        int n = distn(mt);
        auto [a, z] = generate_gauss_double(n, -100, 100);

        auto b = float_inverse(a);
        cnt_degen1 += !b;
        if (!b)
            continue;

        auto c = float_inverse(*b);
        cnt_degen2 += !c;
        if (!c)
            continue;

        cnt_diff_mul += !(min_diff(matd::identity(n), a * *b) <= 1e-16);
        cnt_diff_inv += !(min_diff(a, *c) <= 1e-12);
    }

    auto deg1 = format("{:5.1f}%", 100.0 * cnt_degen1 / runs);
    auto deg2 = format("{:5.1f}%", 100.0 * cnt_degen2 / runs);
    auto diff_mul = format("{:5.1f}%", 100.0 * cnt_diff_mul / runs);
    auto diff_inv = format("{:5.1f}%", 100.0 * cnt_diff_inv / runs);
    printcl(" {} deg1  {} deg2  | bad: {} diff mul  {} diff inv\n", deg1, deg2, diff_mul,
            diff_inv);
}

void stress_test_inverse_modnum() {
    intd distn(5, 100);
    int cnt_degenerate = 0, cnt_diff = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS (5s, now, runs) {
        print_time(now, 5s, "stress test inverse modnum");

        int n = distn(mt);
        auto [a, z] = generate_gauss_modnum(n);

        auto b = inverse(a);
        cnt_degenerate += !b;
        if (!b)
            continue;

        auto c = inverse(*b);
        cnt_degenerate += !c;
        if (!c)
            continue;

        cnt_diff = a * *b != matn::identity(n) || a != *c;
    }

    auto degenerate = format("{:5.1f}%", 100.0 * cnt_degenerate / runs);
    auto diff = format("{:5.1f}%", 100.0 * cnt_diff / runs);
    printcl(" {} degenerate  {} diff\n", degenerate, diff);
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
            auto x = solve_float_linear_system(a, b);
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

    print_time_table(table, "Scaling gauss doubles");
}

void scaling_test_gauss_modnum() {
    static vector<int> Ns = {5, 10, 20, 40, 70, 100, 150, 200, 300, 500};
    vector<vector<stringable>> table;
    table.push_back({"N", "degenerate", "infeasible", "different", "time"});

    for (int N : Ns) {
        string label = format("{0}x{0} scaling test gauss double", N);

        int run_degenerate = 0;
        int run_infeasible = 0;
        int run_different = 0;

        START_ACC(gauss);

        LOOP_FOR_DURATION_TRACKED_RUNS (800ms, now, runs) {
            print_time(now, 800ms, label);

            auto [a, z] = generate_gauss_modnum(N);
            auto b = a * z;

            START(gauss);
            auto x = solve_linear_system(a, b);
            ADD_TIME(gauss);

            run_degenerate += !x;
            if (!x)
                continue;

            bool ok = a * *x == b;
            run_infeasible += !ok;
            run_different += ok && *x != z;
        }

        auto degenerate = format("{:.1f}%", 100.0 * run_degenerate / runs);
        auto infeasible = format("{:.1f}%", 100.0 * run_infeasible / runs);
        auto different = format("{:.1f}%", 100.0 * run_different / runs);
        table.push_back({N, degenerate, infeasible, different, FORMAT_EACH(gauss, runs)});
    }

    print_time_table(table, "Scaling gauss modnum");
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
        matd a({n, n});
        matf b({n, n});
        matn c({n, n});

        for (int i = 0; i < n; i++) {
            for (int j = 0, x; j < n; j++) {
                x = quest[t][i][j];
                a[{i, j}] = x;
                b[{i, j}] = x;
                c[{i, j}] = x;
            }
        }

        cout << b << endl;

        auto [a_det, a_rank] = float_gauss_elimination(a);
        auto [b_det, b_rank] = gauss_elimination(b);
        auto [c_det, c_rank] = gauss_elimination(c);

        debug(a_det, b_det, c_det, ans_det[t]);
        debug(a_rank, b_rank, c_rank, ans_rank[t]);

        assert(abs(a_det - ans_det[t]) < 1e-10);
        assert(b_det == ans_det[t]);
        assert(c_det == ans_det[t]);
        assert(a_rank == ans_rank[t]);
        assert(b_rank == ans_rank[t]);
        assert(c_rank == ans_rank[t]);
    }
}

void unit_test_rank() {
    matd a;

    a = matd::from({
        {1, 3, 5},
        {2, 4, 6},
        {6, 7, 8},
        {1, 2, 3},
        {9, 8, 7},
    });
    print("gauss(a) = {}\n", float_gauss_elimination(a));

    a = matd::from({
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 1, 0, 1},
        {1, 0, 1, 0},
        {1, 1, 1, 0},
    });
    print("gauss(a) = {}\n", float_gauss_elimination(a));

    a = transpose(a);
    print("gauss(transpose(a)) = {}\n", float_gauss_elimination(a));
}

int main() {
    RUN_SHORT(unit_test_gauss_frac());
    RUN_SHORT(unit_test_inverse_frac());
    RUN_SHORT(unit_test_det());
    RUN_SHORT(unit_test_rank());
    RUN_SHORT(stress_test_gauss_modnum());
    RUN_SHORT(stress_test_gauss_double());
    RUN_SHORT(stress_test_inverse_modnum());
    RUN_SHORT(stress_test_inverse_double());
    RUN_BLOCK(scaling_test_gauss_modnum());
    RUN_BLOCK(scaling_test_gauss_double());
    return 0;
}
