#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

#include "frac.hpp"
#include "matrix.hpp"
#include "test/test_utils.hpp"

// *****

enum LPState { LP_FEASIBLE = 0, LP_OPTIMAL = 1, LP_UNBOUNDED = 2, LP_IMPOSSIBLE = 3 };
enum LPConstraintType { LP_LESS = 0, LP_EQUAL = 1, LP_GREATER = 2 };

LPConstraintType flip(LPConstraintType t) { return LPConstraintType(2 * LP_EQUAL - t); }

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

    int n, m;
    mat::vec z;
    vector<lp_constraint> C;

    int num_basis_variables() const { return n; }
    int num_constraints() const { return m; }

    void set_objective(mat::vec f) {
        assert(!n || n == f.size());
        z = move(f), n = z.size();
    }

    void add_constraint(lp_constraint constraint) {
        assert(n && constraint.v.size() == n);
        C.push_back(move(constraint));
        m++;
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

    LPState optimize() {
        do {
            int r = 0, c = min_element(begin(tab[0]) + 1, end(tab[0])) - begin(tab[0]);
            if (tab[0][c] >= 0L) {
                break;
            }
            auto min_ratio = inf;
            for (int i = 1; i <= m; i++) {
                auto ratio = tab[i][0] / tab[i][c];
                if (0L < ratio && ratio < min_ratio) {
                    min_ratio = ratio;
                    r = i;
                }
            }
            if (min_ratio == inf) {
                return LP_UNBOUNDED;
            }

            var_to_row[row_to_var[r]] = 0;
            var_to_row[c] = r, row_to_var[r] = c;

            tab.mul_row(r, 1 / tab[r][c]);
            for (int i = 0; i <= m; i++) {
                if (i != r) {
                    tab.mul_add(i, r, -tab[i][c]);
                }
            }
        } while (true);
        return LP_OPTIMAL;
    }

    pair<LPState, frac> compute() {
        // 0.1. Count slack and artificial variables
        s = a = 0;
        for (int i = 0; i < m; i++) {
            const auto& [v, b, type] = C[i];
            s += type != LP_EQUAL;
            a += (b > 0 && type != LP_LESS) || (b < 0 && type != LP_GREATER);
        }

        // 0.2. Prepare tableau coefficients and basis mappings
        tab = mat(m + 1, n + s + a + 1, 0);
        var_to_row.assign(n + s + a + 1, 0);
        row_to_var.assign(m + 1, 0);

        for (int i = 0, sj = 1 + n, aj = 1 + n + s; i < m; i++) {
            const auto& [v, b, type] = C[i];
            tab[i + 1][0] = b;
            for (int j = 0; j < n; j++) {
                tab[i + 1][j + 1] = v[j];
            }
            // Add a slack variable for inequalities, set it as a basis variable
            if (type != LP_EQUAL) {
                var_to_row[sj] = i + 1, row_to_var[i + 1] = sj;
                tab[i + 1][sj++] = type == LP_LESS ? 1 : -1;
            }
            // Add an artificial variable if necessary
            if ((b > 0 && type != LP_LESS) || (b < 0 && type != LP_GREATER)) {
                var_to_row[aj] = i + 1, row_to_var[i + 1] = aj;
                tab.mul_add(0, i + 1, b > 0 ? -1 : 1); // add to z row
                tab[i + 1][aj++] = b > 0 ? 1 : -1;
            }
        }

        print("n={} m={} s={} a={}\n{}\n", n, m, s, a, to_string(tab));

        // 1. Find initial feasible solution if there are artificial variables
        if (a > 0) {
            // 1.1. Optimize artificial problem
            auto res = optimize();

            // 1.2. Did we find an initial feasible solution?
            if (res != LP_OPTIMAL || tab[0][0] != 0) {
                return {LP_IMPOSSIBLE, tab[0][0]};
            }

            // 1.3. Clear z row and remove artificial columns
            tab.resize(m + 1, n + s + 1);
            tab.set_row(0, 0);
        }

        // 2. Find the optimal solution of the original problem
        // 2.1. Prepare z row
        for (int j = 0; j < n; j++) {
            tab[0][j + 1] = -z[j];
        }

        // 2.2. Expand z row
        for (int j = 0; j < n; j++) {
            if (int i = var_to_row[j + 1]; i != 0) {
                assert(tab[i][j + 1] != 0);
                tab.mul_add(0, i, -tab[0][j + 1] / tab[i][j + 1]);
            }
        }

        // 2.3. Optimize and return the result
        auto res = optimize();
        print("n={} m={} s={} a={}\n{}\n", n, m, s, a, to_string(tab));
        return {res, tab[0][0]};
    }
};

#endif // SIMPLEX_HPP
