#include "test_utils.hpp"
#include "../linear/real_simplex.hpp"
#include "../linear/matrix.hpp"

inline namespace detail {

/**
 * A common implementation for both simplex turned out to be way too annoying to
 * implement and put a considerable burden on copy-pasters, so we duplicate the code.
 * It could be worse.
 */
using ld = long double;

ostream& operator<<(ostream& out, LPState type) { return out << to_string(type); }

template <typename T>
auto generate_lp(int n, int le, int eq, int ge, LPState state = LP_OPTIMAL) {
    int m = le + eq + ge;
    (void)state;

    auto fill = [](vector<T>& v, int minv, int maxv) {
        intd dist(minv, maxv);
        for (auto& f : v) {
            f = dist(mt);
        }
    };

    // First generate a feasible solution
    vector<T> x(n);
    fill(x, 0, 20);

    mat<T> A(m, n);
    for (int i = 0; i < m; i++)
        fill(A.arr[i], -10, 10);

    vector<T> b = A * x;
    vector<T> d(le + ge);
    fill(d, 0, 30);
    d.resize(m, 0); // add 0s

    vector<real_lp_constraint<T>> constraints(m);

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
    vector<T> z(n);
    fill(z, -10, 20, 1);

    real_simplex<T> smp;
    smp.set_objective(z);
    smp.add_constraints(constraints);
    return smp;
}

bool close(ld x, ld y = 0, ld eps = 1e6 * numeric_limits<ld>::epsilon()) {
    return abs(x - y) <= eps;
}

} // namespace detail

template <typename F>
void unit_test_simplex() {
    real_simplex<F> smp;

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, LP_LESS, 120},
        {{1, 4}, LP_LESS, 100},
        {{5, 5}, LP_LESS, 150},
    });
    auto [res, optimum] = smp.compute();
    print("optimum #1: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 105));

    smp.clear();
    smp.set_objective({-1, 2, 1});
    smp.add_constraints({
        {{2, 1, -1}, LP_LESS, 2},
        {{2, -1, 5}, LP_LESS, 6},
        {{4, 1, 1}, LP_LESS, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #2: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 10));

    smp.clear();
    smp.set_objective({10, -32, 8, 5});
    smp.add_constraints({
        {{4, 2, -5, 9}, LP_LESS, 5},
        {{1, -1, 2, -1}, LP_LESS, 3},
        {{2.5, -5, -3, -2}, LP_LESS, 0},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #3: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 381.0 / 13));

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
    print("optimum #4: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 829.0 / 7));

    smp.add_constraint({{4, 3, 2, 1}, LP_LESS, 6});
    vector<F> x, sol = {0, 0, 2.25, 1.5, 0.5, 0, 9.25, 8, 8.5, 0};

    tie(res, optimum) = smp.compute();
    print("optimum #5: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 345.0 / 4));

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-4, -7, -3, 2}, LP_GREATER, -10},
        {{5, 4, 2, 1}, LP_LESS, 25},
        {{-2, -3, 4, -7}, LP_GREATER, -15},
        {{1, 3, 2, 4}, LP_EQUAL, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #6: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 32));

    smp.clear();
    smp.set_objective({10, 8, 6, 4});
    smp.add_constraints({
        {{-2, 6, -4, 2}, LP_GREATER, 3},
        {{5, 4, 2, 1}, LP_LESS, 25},
        {{-2, -3, 4, -7}, LP_GREATER, -15},
        {{1, 3, 2, 4}, LP_EQUAL, 6},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #7: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 32.5));

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
    print("optimum #8: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 17));

    smp.clear();
    smp.set_objective({4, 3});
    smp.add_constraints({
        {{6, 2}, LP_LESS, 120},
        {{1, 4}, LP_LESS, 100},
        {{5, 5}, LP_LESS, 150},
    });
    tie(res, optimum) = smp.compute();
    print("optimum #9.1: {} {:.6f} | {}\n", res, optimum, to_string(smp.extract()));
    assert(res == LP_OPTIMAL && close(optimum, 105));
}

int main() {
    RUN_SHORT(unit_test_simplex<ld>());
    return 0;
}
