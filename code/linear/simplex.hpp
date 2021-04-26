#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "../numeric/bfrac.hpp" // bfrac, pick one
#include "../numeric/frac.hpp"  // frac, pick one
#include "matrix.hpp"           // mat

// *****

enum LPState { LP_FEASIBLE = 0, LP_OPTIMAL = 1, LP_UNBOUNDED = 2, LP_IMPOSSIBLE = 3 };
enum LPConstraintType { LP_LESS = 0, LP_EQUAL = 1, LP_GREATER = 2 };

#define LP_TSW(type, le, eq, ge) (type == LP_EQUAL ? (eq) : type == LP_LESS ? (le) : (ge))

template <typename F>
struct lp_constraint {
    vector<F> v;
    F b;
    LPConstraintType type;
};

template <typename F>
struct simplex {
    using constraint_t = lp_constraint<F>;
    static inline const F inf = F(1, 0);

    int N = 0, M = 0; // num variables / num constraints
    vector<F> z;
    vector<constraint_t> C;

    void set_objective(vector<F> f) {
        assert(!N || N == int(f.size()));
        z = move(f), N = z.size();
    }

    void add_constraint(constraint_t constraint) {
        assert(N && int(constraint.v.size()) == N);
        C.push_back(move(constraint)), M++;
    }

    void add_constraints(const vector<constraint_t>& constraints) {
        for (const auto& constraint : constraints)
            add_constraint(constraint);
    }

    void clear() { N = M = 0, z.clear(), C.clear(); }

    int S = 0, A = 0; // slack and artificial variable count
    mat<F> tab;
    vector<int> var_to_row, row_to_var;

    int slackvar(int i) const { return --i, LP_TSW(C[i].type, 1, 0, -1); }
    int artifvar(int i) const {
        return --i, LP_TSW(C[i].type, -(C[i].b < 0), C[i].b >= 0 ? 1 : -1, C[i].b > 0);
    }
    void make_basic(int r, int c) {
        var_to_row[row_to_var[r]] = 0, var_to_row[c] = r, row_to_var[r] = c;
    }

    void pivot(int r, int c) {
        make_basic(r, c);
        tab.mul_row(r, 1 / tab[r][c]);
        for (int i = 0; i <= M; i++) {
            if (i != r) {
                tab.mul_add_row(i, r, -tab[i][c]);
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
            F best = inf;
            for (int i = 1; i <= M; i++) {
                if (int j = row_to_var[i]; tab[i][c] > 0) {
                    auto ratio = tab[i][0] / tab[i][c];
                    if (0 <= ratio && (ratio < best || (ratio == best && j < bland))) {
                        best = ratio, r = i, bland = j;
                    }
                }
            }
            if (r == 0) {
                return LP_UNBOUNDED;
            }
            pivot(r, c);
        } while (++iterations < 100 * N * M);

        throw runtime_error("oops: simplex::optimize took too many iterations\n");
    }

    pair<LPState, F> compute() {
        S = A = 0;
        for (int i = 1; i <= M; i++) {
            S += slackvar(i) != 0, A += artifvar(i) != 0;
        }

        tab = mat<F>(M + 1, N + S + A + 1, 0);
        var_to_row.assign(N + S + A + 1, 0);
        row_to_var.assign(M + 1, 0);

        for (int i = 1, s = 1 + N, a = 1 + N + S; i <= M; i++) {
            const auto& [v, b, type] = C[i - 1];
            tab[i][0] = b;
            for (int j = 1; j <= N; j++) {
                tab[i][j] = v[j - 1];
            }
            if (int c = slackvar(i); c != 0) {
                make_basic(i, s);
                tab[i][s++] = F(c);
            }
            if (int c = artifvar(i); c != 0) {
                make_basic(i, a);
                tab.mul_add_row(0, i, -F(c));
                tab[i][a++] = F(c);
            }
            if (int j = row_to_var[i]; j != 0 && tab[i][j] != 1) {
                tab.mul_row(i, 1 / tab[i][j]);
            }
        }

        if (A > 0) {
            auto res = optimize();
            if (res != LP_OPTIMAL || tab[0][0] != 0) {
                assert(res == LP_OPTIMAL && tab[0][0] < 0);
                return {LP_IMPOSSIBLE, tab[0][0]};
            }

            for (int a = 1 + N + S; a <= N + S + A; a++) {
                row_to_var[var_to_row[a]] = 0;
            }
            tab.resize(M + 1, N + S + 1);
            for (int i = 1; i <= M; i++) {
                if (row_to_var[i])
                    continue;
                for (int j = 1; j <= N + S; j++) {
                    if (!var_to_row[j] && tab[i][j] != 0 && tab[i][0] / tab[i][j] >= 0) {
                        pivot(i, j);
                        break;
                    }
                }
            }
            tab.set_row(0, 0);
        }

        for (int j = 1; j <= N; j++) {
            tab[0][j] = -z[j - 1];
        }
        for (int j = 1; j <= N; j++) {
            if (int i = var_to_row[j]; i != 0) {
                tab.mul_add_row(0, i, -tab[0][j] * tab[i][j]);
            }
        }

        auto res = optimize();
        return {res, tab[0][0]};
    }

    vector<F> extract() const {
        vector<F> x(N, 0);
        for (int j = 1; j <= N; j++) {
            if (int r = var_to_row[j]; r != 0) {
                x[j - 1] = tab[r][0] / tab[r][j];
            }
        }
        return x;
    }
};

#endif // SIMPLEX_HPP
