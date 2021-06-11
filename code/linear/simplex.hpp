#pragma once

#include <bits/stdc++.h>
using namespace std;

enum LPState { LP_FEASIBLE = 0, LP_OPTIMAL = 1, LP_UNBOUNDED = 2, LP_IMPOSSIBLE = 3 };
enum LPIneq { LP_LESS = 0, LP_EQUAL = 1, LP_GREATER = 2 };

template <typename T>
struct lp_constraint {
    vector<T> v;
    LPIneq ineq;
    T b;
};

template <typename T>
struct simplex {
    int N = 0, M = 0; // num variables / num constraints
    vector<T> z;
    vector<lp_constraint<T>> C;

    explicit simplex(int N = 0) : N(N), z(N, 1) {}

    int add_var(T zcoef = 1) { return z.push_back(zcoef), N++; }

    void set_objective(vector<T> objective) {
        N = max(N, int(objective.size()));
        z = move(objective);
    }
    void add_constraints(const vector<lp_constraint<T>>& constraints) {
        for (const auto& constraint : constraints) {
            add_constraint(constraint);
        }
    }
    void add_constraint(lp_constraint<T> constraint) {
        N = max(N, int(constraint.v.size()));
        C.push_back(move(constraint)), M++;
    }

    void clear() {
        N = M = S = A = 0, z.clear(), C.clear();
        var_row.clear(), row_var.clear();
    }

    int S = 0, A = 0; // slack and artificial variable count
    vector<vector<T>> tab;
    vector<int> var_row, row_var;

    int slackvar(int i) const {
        return (C[i - 1].ineq == LP_LESS) - (C[i - 1].ineq == LP_GREATER);
    }
    int artifvar(int i) const {
        if (C[i - 1].ineq == LP_LESS) {
            return C[i - 1].b < 0 ? -1 : 0;
        } else if (C[i - 1].ineq == LP_EQUAL) {
            return C[i - 1].b >= 0 ? 1 : -1;
        } else {
            return C[i - 1].b > 0 ? 1 : 0;
        }
    }
    void mul_add_row(int i, int src, T mul) {
        for (int j = 0, L = tab[0].size(); j < L; j++) {
            tab[i][j] += mul * tab[src][j];
        }
    }
    void mul_row(int i, T mul) {
        for (int j = 0, L = tab[0].size(); j < L; j++) {
            tab[i][j] = mul * tab[i][j];
        }
    }
    void set_row(int i, T num) {
        for (int j = 0, L = tab[0].size(); j < L; j++) {
            tab[i][j] = num;
        }
    }

    void make_basic(int r, int c) {
        var_row[row_var[r]] = 0, var_row[c] = r, row_var[r] = c;
    }
    void pivot(int r, int c) {
        make_basic(r, c);
        mul_row(r, 1 / tab[r][c]);
        for (int i = 0; i <= M; i++) {
            if (i != r) {
                mul_add_row(i, r, -tab[i][c]);
            }
        }
    }

    LPState optimize() {
        int iterations = 0;
        do {
            int r = 0, c = min_element(begin(tab[0]) + 1, end(tab[0])) - begin(tab[0]);
            if (tab[0][c] >= 0) {
                return LP_OPTIMAL;
            }
            int bland = INT_MAX;
            T best = 0;
            for (int i = 1; i <= M; i++) {
                if (int j = row_var[i]; tab[i][c] > 0) {
                    if (auto ratio = tab[i][0] / tab[i][c]; ratio >= 0) {
                        if (r == 0 || ratio < best || (ratio <= best && j < bland)) {
                            best = ratio, r = i, bland = j;
                        }
                    }
                }
            }
            if (r == 0) {
                return LP_UNBOUNDED;
            }
            pivot(r, c);
        } while (++iterations < 100 * N * M);

        throw runtime_error("oops: simplex::optimize took too many iterations!");
    }

    pair<LPState, T> compute() {
        S = A = 0;
        for (int i = 1; i <= M; i++) {
            S += slackvar(i) != 0;
            A += artifvar(i) != 0;
        }

        tab.assign(M + 1, vector<T>(N + S + A + 1, T(0)));
        var_row.assign(N + S + A + 1, 0);
        row_var.assign(M + 1, 0);

        for (int i = 1, s = 1 + N, a = 1 + N + S; i <= M; i++) {
            const auto& v = C[i - 1].v;
            auto b = C[i - 1].b;
            tab[i][0] = b;
            for (int j = 1, L = v.size(); j <= L; j++) {
                tab[i][j] = v[j - 1];
            }
            if (int c = slackvar(i); c != 0) {
                make_basic(i, s);
                tab[i][s++] = T(c);
            }
            if (int c = artifvar(i); c != 0) {
                make_basic(i, a);
                mul_add_row(0, i, -T(c));
                tab[i][a++] = T(c);
            }
            if (int j = row_var[i]; j != 0 && tab[i][j] != 1) {
                mul_row(i, 1 / tab[i][j]);
            }
        }

        if (A > 0) {
            auto res = optimize();
            if (res != LP_OPTIMAL || tab[0][0] != 0) {
                assert(res == LP_OPTIMAL && tab[0][0] < 0); // internal bug or overflow
                return {LP_IMPOSSIBLE, tab[0][0]};
            }

            for (int a = 1 + N + S; a <= N + S + A; a++) {
                row_var[var_row[a]] = 0;
            }
            for (int i = 0; i <= M; i++) {
                tab[i].resize(N + S + 1);
            }
            for (int i = 1; i <= M; i++) {
                if (row_var[i])
                    continue;
                for (int j = 1; j <= N + S; j++) {
                    if (!var_row[j] && tab[i][j] != 0 && tab[i][0] / tab[i][j] >= 0) {
                        pivot(i, j);
                        break;
                    }
                }
            }
            set_row(0, 0);
        }

        for (int j = 1; j <= N; j++) {
            tab[0][j] = -z[j - 1];
        }
        for (int j = 1; j <= N; j++) {
            if (int i = var_row[j]; i != 0) {
                mul_add_row(0, i, -tab[0][j] * tab[i][j]);
            }
        }

        auto res = optimize();
        return {res, tab[0][0]};
    }

    vector<T> extract() const {
        vector<T> x(N, 0);
        for (int j = 1; j <= N; j++) {
            if (int r = var_row[j]; r != 0) {
                x[j - 1] = tab[r][0] / tab[r][j];
            }
        }
        return x;
    }
};

template <typename T>
tuple<LPState, T, vector<T>> solve_lp(const vector<T>& objective,
                                      const vector<lp_constraint<T>>& A) {
    int N = objective.size();
    simplex<T> solver(N);
    solver.set_objective(objective);
    solver.set_constraints(A);
    auto [state, value] = solver.optimize();
    if (state == LP_OPTIMAL || state == LP_UNBOUNDED) {
        return {state, value, solver.extract()};
    } else {
        return {state, value, {}};
    }
}
