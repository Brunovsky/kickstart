#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "frac.hpp"
#include "matrix.hpp"
#include "test/test_utils.hpp"

// *****

enum LPState { LP_FEASIBLE = 0, LP_OPTIMAL = 1, LP_UNBOUNDED = 2, LP_IMPOSSIBLE = 3 };
enum LPConstraintType { LP_LESS = 0, LP_EQUAL = 1, LP_GREATER = 2 };

struct lp_constraint {
    vector<frac> v;
    frac b;
    LPConstraintType type;
};

/**
 *      (n) constraints     (s) slack vars      (a) artificial vars
 * +---+-------------------+-------------------+-------+
 * | 0 |-z0|-z1|-z2|-z3|-z4| 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 * | b0|a00|a01|a02|a03|a04| 1 | 0 | 0 | 0 | 0 | 0 | 0 |
 * | b1|a10|a11|a12|a13|a14| 0 | 1 | 0 | 0 | 0 | 0 | 0 |
 * | b2|a20|a21|a22|a23|a24| 0 | 0 |-1 | 0 | 0 | 0 | 0 |
 * | b3|a30|a31|a32|a33|a34| 0 | 0 | 0 | 1 | 0 | 0 | 1 |
 * | b4|a40|a41|a42|a43|a44| 0 | 0 | 0 | 0 | 1 | 0 | 0 |
 * | b5|a50|a51|a52|a53|a54| 0 | 0 | 0 | 0 | 0 |-1 | 0 |
 */
struct simplex {
    using mat = mat<frac>;
    static inline constexpr frac inf = frac(1, 0);

    int n = 0, m = 0; // num variables / num constraints
    mat::vec z;
    vector<lp_constraint> C;

    void set_objective(mat::vec f) {
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
    mat tab;
    vector<int> var_to_row;
    vector<int> row_to_var;

    void pivot(int r, int c) {
        var_to_row[row_to_var[r]] = 0;
        var_to_row[c] = r, row_to_var[r] = c;

        tab.mul_row(r, 1 / tab[r][c]);
        for (int i = 0; i <= m; i++) {
            if (i != r) {
                tab.mul_add(i, r, -tab[i][c]);
            }
        }
    }

    LPState optimize() {
        do {
            int r = 0, c = min_element(begin(tab[0]) + 1, end(tab[0])) - begin(tab[0]);
            if (tab[0][c] >= 0L) {
                return LP_OPTIMAL;
            }
            frac min_ratio = -1;
            for (int i = 1; i <= m; i++) {
                if (tab[i][c] > 0) {
                    auto ratio = tab[i][0] / tab[i][c];
                    if (0L <= ratio && (ratio < min_ratio || r == 0)) {
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
        for (int i = 0; i < m; i++) {
            const auto& [v, b, type] = C[i];
            s += type != LP_EQUAL;
            a += (b > 0 && type != LP_LESS) || (b < 0 && type != LP_GREATER);
        }

        tab = mat(m + 1, n + s + a + 1, 0);
        var_to_row.assign(n + s + a + 1, 0);
        row_to_var.assign(m + 1, 0);

        for (int i = 1, sj = 1 + n, aj = 1 + n + s; i <= m; i++) {
            const auto& [v, b, type] = C[i - 1];
            tab[i][0] = b;
            for (int j = 1; j <= n; j++) {
                tab[i][j] = v[j - 1];
            }
            if (type != LP_EQUAL) {
                var_to_row[sj] = i, row_to_var[i] = sj;
                tab[i][sj++] = type == LP_LESS ? 1 : -1;
            }
            if ((b > 0 && type != LP_LESS) || (b < 0 && type != LP_GREATER)) {
                var_to_row[row_to_var[i]] = 0;
                var_to_row[aj] = i, row_to_var[i] = aj;
                tab.mul_add(0, i, b > 0 ? -1 : 1);
                tab[i][aj++] = b > 0 ? 1 : -1;
            }
            if (row_to_var[i] != 0 && tab[i][row_to_var[i]] != 0) {
                tab.mul_row(i, 1 / tab[i][row_to_var[i]]);
            }
        }

        if (a > 0) {
            auto res = optimize();

            if (res != LP_OPTIMAL || tab[0][0] != 0) {
                assert(tab[0][0] <= 0);
                return {LP_IMPOSSIBLE, tab[0][0]};
            }

            for (int v = 0; v < a; v++) {
                if (int i = var_to_row[1 + n + s + v]; i > 0) {
                    row_to_var[i] = 0;
                    for (int j = 1; j <= n + s; j++) {
                        if (var_to_row[j] == 0 && tab[i][j] != 0) {
                            pivot(i, j);
                            break;
                        }
                    }
                }
            }

            tab.resize(m + 1, n + s + 1);
            tab.set_row(0, 0);
        }

        for (int j = 1; j <= n; j++) {
            tab[0][j] = -z[j - 1];
        }
        for (int j = 1; j <= n; j++) {
            if (int i = var_to_row[j]; i != 0) {
                assert(tab[i][j] != 0);
                tab.mul_add(0, i, -tab[0][j] * tab[i][j]);
            }
        }

        auto res = optimize();
        return {res, tab[0][0]};
    }
};

#endif // SIMPLEX_HPP
