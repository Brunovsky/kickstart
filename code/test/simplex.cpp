#include "test_utils.hpp"
#include "../linear/simplex.hpp"
#include "../lib/anynum.hpp"
#include "../linear/simplex_utils.hpp"
#include "../linear/matrix.hpp"
#include "../numeric/bfrac.hpp"
#include "../numeric/frac.hpp"

inline namespace detail {

template <typename F>
string f_name;
template <>
string f_name<frac> = "frac";
template <>
string f_name<bfrac> = "bfrac";

ostream& operator<<(ostream& out, LPState type) { return out << to_string(type); }

template <typename F>
auto compute_slack(const simplex<F>& smp, const vector<F>& x) {
    vector<F> slack(smp.M, 0);
    for (int i = 0; i < smp.M; i++) {
        F c = 0;
        for (int j = 0; j < smp.N; j++) {
            c += x[j] * smp.C[i].v[j];
        }
        slack[i] = smp.C[i].b - c;
        assert(slack[i] >= 0 && smp.C[i].ineq == LP_LESS);
    }
    return slack;
}

template <typename F>
bool complementary_slackness(const simplex<F>& primal, const simplex<F>& dual) {
    auto x = primal.extract(), y = dual.extract();
    assert(is_feasible(primal, x) && is_feasible(dual, y));
    auto slack_x = compute_slack(primal, x);
    auto slack_y = compute_slack(dual, y);
    bool ok = true;
    for (int i = 0; i < primal.N && ok; i++) {
        ok = x[i] == 0 || slack_y[i] == 0;
    }
    for (int i = 0; i < dual.N && ok; i++) {
        ok = y[i] == 0 || slack_x[i] == 0;
    }
    return ok;
}

template <typename F>
auto generate_lp(int n, int le, int eq, int ge, LPState state = LP_OPTIMAL) {
    int m = le + eq + ge;
    (void)state;

    auto fill = [](vector<F>& v, int minv, int maxv, int maxd) {
        for (auto& f : v)
            f = uniform_gen<F>(minv, maxv, maxd);
    };

    // First generate a feasible solution
    vector<F> x(n);
    fill(x, 0, 9, 2);

    mat<F> A(m, n);
    for (int i = 0; i < m; i++)
        fill(A.arr[i], -5, 5, 1);

    vector<F> b = A * x;
    vector<F> d(le + ge);
    fill(d, 0, 10, 1);
    d.resize(m, 0); // add 0s

    vector<lp_constraint<F>> constraints(m);

    for (int i = 0; i < m; i++) {
        constraints[i].v = move(A.arr[i]);
        if (le > 0) {
            le--, constraints[i].b = b[i] + d[i], constraints[i].ineq = LP_LESS;
        } else if (ge > 0) {
            ge--, constraints[i].b = b[i] - d[i], constraints[i].ineq = LP_GREATER;
        } else if (eq > 0) {
            eq--, constraints[i].b = b[i], constraints[i].ineq = LP_EQUAL;
        } else {
            assert(false);
        }
    }

    shuffle(begin(constraints), end(constraints), mt);

    // Generate a random objective function
    vector<F> z(n);
    fill(z, -10, 20, 1);

    simplex<F> smp;
    smp.set_objective(z);
    smp.add_constraints(constraints);
    return smp;
}

} // namespace detail

inline namespace unit_testing_simplex {

template <typename F>
void unit_test_simplex() {
    simplex<F> smp;

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, LP_LESS, 120},
        {{1, 4}, LP_LESS, 100},
        {{5, 5}, LP_LESS, 150},
    });
    auto [res, optimum] = smp.compute();
    print("optimum #1: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == 105);

    smp.clear();
    smp.set_objective({-1, 2, 1});
    smp.add_constraints({
        {{2, 1, -1}, LP_LESS, 2},
        {{2, -1, 5}, LP_LESS, 6},
        {{4, 1, 1}, LP_LESS, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #2: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == 10);

    smp.clear();
    smp.set_objective({10, -32, 8, 5});
    smp.add_constraints({
        {{4, 2, -5, 9}, LP_LESS, 5},
        {{1, -1, 2, -1}, LP_LESS, 3},
        {{F(5, 2), -5, -3, -2}, LP_LESS, 0},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #3: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == F(381, 13));

    smp.clear();
    smp.set_objective({7, -19, 31, 11});
    smp.add_constraints({
        {{4, 2, 8, -7}, LP_LESS, 8},
        {{1, -1, 2, -1}, LP_LESS, 3},
        {{3, 3, -1, 0}, LP_LESS, 7},
        {{0, -4, 2, -3}, LP_LESS, 8},
        {{-2, 7, -4, 9}, LP_LESS, 13},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #4: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == F(829, 7));

    smp.add_constraint({{4, 3, 2, 1}, LP_LESS, 6});
    vector<F> x, sol = {0, 0, F(9, 4), F(3, 2), F(1, 2), 0, F(37, 4), 8, F(17, 2), 0};

    tie(res, optimum) = smp.compute();
    print("optimum #5: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == F(345, 4));

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-4, -7, -3, 2}, LP_GREATER, -10},
        {{5, 4, 2, 1}, LP_LESS, 25},
        {{-2, -3, 4, -7}, LP_GREATER, -15},
        {{1, 3, 2, 4}, LP_EQUAL, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #6: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == 32);

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-2, 6, -4, 2}, LP_GREATER, 3},
        {{5, 4, 2, 1}, LP_LESS, 25},
        {{-2, -3, 4, -7}, LP_GREATER, -15},
        {{1, 3, 2, 4}, LP_EQUAL, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #7: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == F(65, 2));

    smp.clear();
    smp.set_objective({1, -2, 3, 4, -5, 6});
    smp.add_constraints({
        {{1, 1, 1, 2, 2, 2}, LP_LESS, 9},
        {{3, 3, -3, -1, 1, 1}, LP_GREATER, 13},
        {{1, 2, 3, 4, 5, 6}, LP_EQUAL, 17},
        {{2, -2, 3, -3, 4, -4}, LP_EQUAL, 0},
        {{-7, 3, 2, 4, -1, -8}, LP_LESS, -12},
        {{4, -2, -6, -11, 3, -2}, LP_GREATER, -4},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #8: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == 17);

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, LP_LESS, 120},
        {{1, 4}, LP_LESS, 100},
        {{5, 5}, LP_LESS, 150},
    });
    smp = standardize(smp);
    tie(res, optimum) = smp.compute();
    print("optimum #9.1: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == 105);

    smp = make_dual(smp);
    tie(res, optimum) = smp.compute();
    print("optimum #9.2: {} {} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && optimum == -105);
}

} // namespace unit_testing_simplex

inline namespace stress_testing_simplex {

template <typename F>
void stress_test_standardize_run(int n, int m) {
    int good = 0, unbounded = 0;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(700ms, now, 1000, runs) {
        print_time(now, 700ms, 50ms, "simplex standardize test");

        auto parts = partition_sample(m, 3, 0);
        int le = parts[0], eq = parts[1], ge = parts[2];
        auto smp = generate_lp<F>(n, le, eq, ge, LP_OPTIMAL);
        auto std = standardize(smp);
        auto dual = make_dual(standardize(smp));

        auto [res, opt] = smp.compute();
        auto [res2, opt2] = std.compute();
        auto [res3, opt3] = dual.compute();

        auto x1 = smp.extract();
        auto x2 = std.extract();
        auto x3 = dual.extract();

        bool ok = true;
        ok = ok && res == res2;
        ok = ok && is_feasible(smp, x1) && is_feasible(std, x1);
        ok = ok && is_feasible(smp, x2) && is_feasible(std, x2);
        if (res == LP_OPTIMAL) {
            ok = ok && opt == opt2 && res3 == LP_OPTIMAL && opt == -opt3;
            ok = ok && is_feasible(dual, x3);
            ok = ok && complementary_slackness(std, dual);
        } else if (res == LP_UNBOUNDED) {
            ok = ok && res3 == LP_IMPOSSIBLE;
            unbounded++;
        }

        if (!ok) {
            print("optimums: {} {} / {} {} / {} {}\n", res, opt, res2, opt2, res3, opt3);
            print("  smp, x1 feasible: {}\n", is_feasible(smp, x1));
            print("  std, x1 feasible: {}\n", is_feasible(std, x1));
            print("  smp, x2 feasible: {}\n", is_feasible(smp, x2));
            print("  std, x2 feasible: {}\n", is_feasible(std, x2));
            print(" dual, x3 feasible: {}\n", is_feasible(dual, x3));
            print("{}\n", format_simplex(smp));
            print("{}\n", format_tableau(smp));
        }

        good += ok;
    }

    double good_percent = 100.0 * good / runs;
    double unbounded_percent = 100.0 * unbounded / runs;
    printcl(" n={:<2} m={:<2} correct: {:>5}/{:<5} ({:.1f}%) | unbounded: "
            "{:>5}/{:<5} ({:.1f}%) \n",
            n, m, good, runs, good_percent, unbounded, runs, unbounded_percent);
}

template <typename F>
void stress_test_standardize(int lo = 2, int hi = 20, int max_sum = 30) {
    for (int n = lo; n <= hi; n++) {
        for (int m = lo; m <= hi && n + m <= max_sum; m++) {
            stress_test_standardize_run<F>(n, m);
        }
    }
}

} // namespace stress_testing_simplex

inline namespace speed_testing_simplex {

template <typename F>
void speed_test_simplex_run(int n, int m, LPState state) {
    START_ACC(simplex);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(700ms, now, 1000, runs) {
        print_time(now, 700ms, 50ms, "simplex speed test");

        auto parts = partition_sample(m, 3, 0);
        int le = parts[0], eq = parts[1], ge = parts[2];
        auto smp = generate_lp<F>(n, le, eq, ge, state);

        START(simplex);
        smp.compute();
        ADD_TIME(simplex);
    }

    printcl(" {:>8}ms -- x{} n={:<2} m={:<2}\n", EACH_MS(simplex, runs), runs, n, m);
}

template <typename F>
void speed_test_simplex(int lo = 2, int hi = 20, int max_sum = 30) {
    for (int n = lo; n <= hi; n++) {
        for (int m = lo; m <= hi && n + m <= max_sum; m++) {
            speed_test_simplex_run<F>(n, m, LP_OPTIMAL);
        }
    }
}

} // namespace speed_testing_simplex

int main() {
    RUN_SHORT(unit_test_simplex<frac>());
    RUN_SHORT(unit_test_simplex<bfrac>());
    RUN_BLOCK(stress_test_standardize<frac>(2, 12, 15));
    RUN_BLOCK(stress_test_standardize<bfrac>(2, 25, 30));
    RUN_BLOCK(speed_test_simplex<frac>(2, 12, 15));
    RUN_BLOCK(speed_test_simplex<bfrac>(2, 25, 30));
    return 0;
}
