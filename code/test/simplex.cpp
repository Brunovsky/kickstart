#include "../simplex.hpp"

#include "test_utils.hpp"

// *****

#define F frac

string to_string(const vector<frac>& x) {
    string s;
    for (auto f : x) {
        s += " " + to_string(f);
    }
    return s;
}

string to_string(LPState type) {
    static const string names[] = {"feasible", "optimal", "unbounded", "impossible"};
    return names[int(type)];
}
ostream& operator<<(ostream& out, LPState type) { return out << to_string(type); }

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
    print("optimum #7: {} {}\n", res, optimum);
    assert(res == LP_OPTIMAL && optimum == 17L);
}

int main() {
    unit_test_simplex();
    return 0;
}
