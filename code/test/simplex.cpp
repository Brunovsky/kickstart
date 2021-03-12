#include "../simplex.hpp"

#include "../random.hpp"
#include "../simplex_utils.hpp"
#include "test_utils.hpp"

// *****

#define F frac

string to_string(LPState type) {
    static const string names[] = {"feasible", "optimal", "unbounded", "impossible"};
    return names[int(type)];
}
ostream& operator<<(ostream& out, LPState type) { return out << to_string(type); }

auto generate_lp(int n, int le, int eq, int ge, LPState state = LP_OPTIMAL) {
    int m = le + eq + ge;
    (void)state;

    auto fill = [](vector<frac>& v, int minv, int maxv, int maxd) {
        intd dist(minv * maxd, maxv * maxd);
        for (auto& f : v)
            f = frac(dist(mt), maxd);
    };

    // First generate a feasible solution
    vector<frac> x(n);
    fill(x, 0, 9, 2);

    mat<frac> A(m, n);
    for (int i = 0; i < m; i++)
        fill(A.arr[i], 0, 8, 1);

    vector<frac> b = A * x;
    vector<frac> d(le + ge);
    fill(d, 0, 10, 1);
    d.resize(m, 0); // add 0s

    vector<lp_constraint> constraints(m);

    for (int i = 0; i < m; i++) {
        constraints[i].v = move(A.arr[i]);
        if (le > 0) {
            le--, constraints[i].b = b[i] + d[i], constraints[i].type = LP_LESS;
        } else if (eq > 0) {
            eq--, constraints[i].b = b[i], constraints[i].type = LP_EQUAL;
        } else if (ge > 0) {
            ge--, constraints[i].b = b[i] - d[i], constraints[i].type = LP_GREATER;
        } else {
            assert(false);
        }
    }

    // Generate a random objective function
    vector<frac> z(n);
    fill(z, -10, 20, 1);

    simplex smp;
    smp.set_objective(z);
    smp.add_constraints(constraints);
    return smp;
}

bool operator==(const lp_constraint& a, const lp_constraint& b) {
    return a.b == b.b && a.v == b.v && a.type == b.type;
}
bool operator==(const simplex& a, const simplex& b) {
    return a.n == b.n && a.m == b.m && a.z == b.z && a.C == b.C;
}

void unit_test_simplex() {
    simplex smp;

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, 120, LP_LESS},
        {{1, 4}, 100, LP_LESS},
        {{5, 5}, 150, LP_LESS},
    });
    auto [res, optimum] = smp.compute();
    print("optimum #1: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 105L);

    smp.clear();
    smp.set_objective({-1, 2, 1});
    smp.add_constraints({
        {{2, 1, -1}, 2, LP_LESS},
        {{2, -1, 5}, 6, LP_LESS},
        {{4, 1, 1}, 6, LP_LESS},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #2: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 10L);

    smp.clear();
    smp.set_objective({10, -32, 8, 5});
    smp.add_constraints({
        {{4, 2, -5, 9}, 5, LP_LESS},
        {{1, -1, 2, -1}, 3, LP_LESS},
        {{F(5, 2), -5, -3, -2}, 0, LP_LESS},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #3: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == F(381, 13));

    smp.clear();
    smp.set_objective({7, -19, 31, 11});
    smp.add_constraints({
        {{4, 2, 8, -7}, 8, LP_LESS},
        {{1, -1, 2, -1}, 3, LP_LESS},
        {{3, 3, -1, 0}, 7, LP_LESS},
        {{0, -4, 2, -3}, 8, LP_LESS},
        {{-2, 7, -4, 9}, 13, LP_LESS},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #4: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == F(829, 7));

    smp.add_constraint({{4, 3, 2, 1}, 6, LP_LESS});
    vector<frac> x, sol = {0, 0, F(9, 4), F(3, 2), F(1, 2), 0, F(37, 4), 8, F(17, 2), 0};

    tie(res, optimum) = smp.compute();
    print("optimum #5: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == F(345, 4));
    // print("{} | {}\n", to_string(x), to_string(sol));

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-4, -7, -3, 2}, -10, LP_GREATER},
        {{5, 4, 2, 1}, 25, LP_LESS},
        {{-2, -3, 4, -7}, -15, LP_GREATER},
        {{1, 3, 2, 4}, 6, LP_EQUAL},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #6: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 32L);

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-2, 6, -4, 2}, 3, LP_GREATER},
        {{5, 4, 2, 1}, 25, LP_LESS},
        {{-2, -3, 4, -7}, -15, LP_GREATER},
        {{1, 3, 2, 4}, 6, LP_EQUAL},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #7: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == F(65, 2));

    smp.clear();
    smp.set_objective({1, -2, 3, 4, -5, 6});
    smp.add_constraints({
        {{1, 1, 1, 2, 2, 2}, 9, LP_LESS},
        {{3, 3, -3, -1, 1, 1}, 13, LP_GREATER},
        {{1, 2, 3, 4, 5, 6}, 17, LP_EQUAL},
        {{2, -2, 3, -3, 4, -4}, 0, LP_EQUAL},
        {{-7, 3, 2, 4, -1, -8}, -12, LP_LESS},
        {{4, -2, -6, -11, 3, -2}, -4, LP_GREATER},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #8: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 17L);

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, 120, LP_LESS},
        {{1, 4}, 100, LP_LESS},
        {{5, 5}, 150, LP_LESS},
    });
    smp = standardize(smp);
    tie(res, optimum) = smp.compute();
    print("optimum #9.1: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 105);

    smp = make_dual(smp);
    tie(res, optimum) = smp.compute();
    print("optimum #9.2: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == -105);
}

void standardize_test(int T, int n, int m) {
    int correct = 0;

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "simplex standardize test");

        auto parts = partition_sample(m, 3, 0);
        int le = parts[0], eq = parts[1], ge = parts[2];
        auto smp = generate_lp(n, le, eq, ge, LP_OPTIMAL);
        auto std = standardize(smp);
        auto dual = make_dual(standardize(smp));

        auto [res, opt] = smp.compute();
        auto [res2, opt2] = std.compute();
        auto [res3, opt3] = dual.compute();

        bool ok = res == res2 && opt == opt2;
        if (res == LP_OPTIMAL) {
            ok &= res3 == LP_OPTIMAL && opt == -opt3;
        } else if (res == LP_UNBOUNDED) {
            ok &= res3 == LP_IMPOSSIBLE;
        }
        correct += ok;
    }

    print("\n correct: {}/{} ({:.1f}%)\n", correct, T, 100.0 * correct / T);
}

void speed_test_simplex(int T, int n, int m, LPState state) {
    START_ACC(simplex);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "simplex speed test");

        auto parts = partition_sample(m, 3, 0);
        int le = parts[0], eq = parts[1], ge = parts[2];
        auto smp = generate_lp(n, le, eq, ge, state);

        START(simplex);
        smp.compute();
        ADD_TIME(simplex);
    }

    print("x{} n={} m={} {}\n", T, n, m, to_string(state));
    PRINT_TIME(simplex);
}

int main() {
    unit_test_simplex();
    standardize_test(3000, 5, 6);
    speed_test_simplex(300, 5, 6, LP_OPTIMAL);
    return 0;
}
