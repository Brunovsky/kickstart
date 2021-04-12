#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "../numeric/frac.hpp"
#include "matrix.hpp"

// *****

enum LPState { LP_FEASIBLE = 0, LP_OPTIMAL = 1, LP_UNBOUNDED = 2, LP_IMPOSSIBLE = 3 };
enum LPConstraintType { LP_LESS = 0, LP_EQUAL = 1, LP_GREATER = 2 };

#define LP_TSW(type, le, eq, ge) (type == LP_EQUAL ? (eq) : type == LP_LESS ? (le) : (ge))

struct lp_constraint {
    vector<frac> v;
    frac b;
    LPConstraintType type;
};

struct simplex {
    static inline constexpr frac inf = frac(1, 0);

    int n = 0, m = 0; // num variables / num constraints
    mat<frac>::vec z;
    vector<lp_constraint> C;

    void set_objective(mat<frac>::vec f) {
        assert(!n || n == f.size());
        z = move(f), n = z.size();
    }

    void add_constraint(lp_constraint constraint) {
        assert(n && constraint.v.size() == n);
        C.push_back(move(constraint)), m++;
    }

    void add_constraints(const vector<lp_constraint>& constraints) {
        for (const auto& constraint : constraints)
            add_constraint(constraint);
    }

    void clear() { n = m = 0, z.clear(), C.clear(); }

    int s = 0, a = 0; // slack and artificial variable count
    mat<frac> tab;
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
        for (int i = 0; i <= m; i++) {
            if (i != r) {
                tab.mul_add_row(i, r, -tab[i][c]);
            }
        }
    }

    LPState optimize() {
        do {
            int r = 0, c = min_element(begin(tab[0]) + 1, end(tab[0])) - begin(tab[0]);
            if (tab[0][c] >= 0L) {
                return LP_OPTIMAL;
            }
            frac min_ratio = inf;
            for (int i = 1; i <= m; i++) {
                if (tab[i][c] > 0) {
                    auto ratio = tab[i][0] / tab[i][c];
                    if (0L <= ratio && ratio < min_ratio) {
                        min_ratio = ratio, r = i;
                    }
                }
            }
            if (r == 0) {
                return LP_UNBOUNDED;
            }
            pivot(r, c);
        } while (true);
    }

    pair<LPState, frac> compute() {
        s = a = 0;
        for (int i = 1; i <= m; i++) {
            s += slackvar(i) != 0, a += artifvar(i) != 0;
        }

        tab = mat<frac>(m + 1, n + s + a + 1, 0);
        var_to_row.assign(n + s + a + 1, 0);
        row_to_var.assign(m + 1, 0);
        for (int i = 1, sj = 1 + n, aj = 1 + n + s; i <= m; i++) {
            const auto& [v, b, type] = C[i - 1];
            tab[i][0] = b;
            for (int j = 1; j <= n; j++) {
                tab[i][j] = v[j - 1];
            }
            if (slackvar(i)) {
                make_basic(i, sj);
                tab[i][sj++] = slackvar(i);
            }
            if (artifvar(i)) {
                make_basic(i, aj);
                tab.mul_add_row(0, i, frac(-artifvar(i)));
                tab[i][aj++] = artifvar(i);
            }
            if (int j = row_to_var[i]; j != 0) {
                tab.mul_row(i, 1 / tab[i][j]);
            }
        }

        if (a > 0) {
            auto res = optimize();
            if (res != LP_OPTIMAL || tab[0][0] != 0) {
                assert(res == LP_OPTIMAL && tab[0][0] < 0);
                return {LP_IMPOSSIBLE, tab[0][0]};
            }

            for (int v = 1 + n + s; v <= n + s + a; v++) {
                row_to_var[var_to_row[v]] = 0;
            }
            tab.resize(m + 1, n + s + 1);
            for (int i = 1; i <= m; i++) {
                if (!row_to_var[i]) {
                    for (int j = 1; j <= n + s; j++) {
                        if (!var_to_row[j] && tab[i][j] != 0 &&
                            tab[i][0] / tab[i][j] >= 0) {
                            pivot(i, j);
                            break;
                        }
                    }
                }
            }
            tab.set_row(0, frac(0));
        }

        for (int j = 1; j <= n; j++) {
            tab[0][j] = -z[j - 1];
        }
        for (int j = 1; j <= n; j++) {
            if (int i = var_to_row[j]; i != 0) {
                tab.mul_add_row(0, i, -tab[0][j] * tab[i][j]);
            }
        }

        auto res = optimize();
        return {res, tab[0][0]};
    }

    vector<frac> extract() const {
        vector<frac> x(n, 0);
        for (int j = 1; j <= n; j++) {
            if (int r = var_to_row[j]; r != 0) {
                x[j - 1] = tab[r][0] / tab[r][j];
            }
        }
        return x;
    }
};

#endif // SIMPLEX_HPP
