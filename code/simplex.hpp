#include <bits/stdc++.h>

#include "frac.hpp"

using namespace std;

// *****

/**
 * maximize f(x)
 * subject to
 *   Ax <= b
 *   x >= 0
 *   b >= 0
 * n basis variables (size of x and c)
 * m constraints (size of b)
 * the tableau has m+1 rows and n+m+1 columns
 */
struct simplex {
    int n, m; // n: number of basis variables; m: number of constraints
    vector<frac> f;
    vector<vector<frac>> A;
    vector<frac> b;

    simplex(int n) : n(n), m(0) {}

    void add_constraint(vector<frac> Ai, frac bi) {
        assert(int(Ai.size()) == n && bi >= 0L);
        A.push_back(move(Ai));
        b.push_back(bi);
        m++;
    }

    void set_objective(vector<frac> objective) {
        assert(int(objective.size()) == n);
        f = move(objective);
    }

    vector<vector<frac>> tab;
    enum State { FOUND = 0, OPTIMAL = 1, UNBOUNDED = 2 };
    static const frac inf;

    void make_tableau() {
        tab.assign(m + 1, vector<frac>(n + m + 1, 0L));
        for (int j = 0; j < n; j++) {
            tab[0][j + 1] = -f[j];
        }
        for (int i = 0; i < m; i++) {
            tab[i + 1][0] = b[i];
            for (int j = 0; j < n; j++) {
                tab[i + 1][j + 1] = A[i][j];
            }
            tab[i + 1][i + n + 1] = 1; // slack variable
        }
    }

    void pivot(int r, int c) {
        assert(r > 0 && c > 0);
        auto pivot = tab[r][c];

        for (int j = 0; j <= n + m; j++) {
            tab[r][j] /= pivot;
        }
        for (int i = 0; i <= m; i++) {
            if (i == r) {
                continue;
            }
            auto k = tab[i][c];
            for (int j = 0; j <= n + m; j++) {
                tab[i][j] -= k * tab[r][j];
            }
        }
    }

    State select_pivot(int& r, int& c) {
        r = c = 1;
        auto fmax = tab[0][1];
        for (int j = 2; j <= n + m; j++) {
            if (fmax > tab[0][j]) {
                fmax = tab[0][j];
                c = j;
            }
        }
        if (fmax >= 0L) {
            return OPTIMAL;
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
            return UNBOUNDED;
        }
        return FOUND;
    }

    frac compute() {
        make_tableau();
        do {
            int r, c;
            auto state = select_pivot(r, c);
            if (state == FOUND) {
                pivot(r, c);
            } else if (state == UNBOUNDED) {
                return inf;
            } else {
                break;
            }
        } while (true);
        return tab[0][0];
    }
};
const frac simplex::inf(1, 0);

ostream& operator<<(ostream& out, const vector<vector<frac>>& tab) {
    int m = tab.size() - 1;
    int n = tab[0].size() - m - 1;
    vector<size_t> width(n + m + 1);
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n + m; j++) {
            width[j] = max(width[j], 1 + to_string(tab[i][j]).length());
        }
    }
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n + m; j++) {
            out << setw(width[j]) << tab[i][j] << ' ';
        }
        out << '\n';
    }
    return out << '\n';
}