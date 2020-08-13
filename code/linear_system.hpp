#include "frac.hpp"

// *****

struct linear_system {
    vector<vector<frac>> A;
    vector<frac> b;

    // fill in the matrices...

    vector<frac> x;
    enum Result { SOLVED = 0, IMPOSSIBLE = 1, DEGENERATE = 2 };

    Result compute() {
        vector<vector<frac>> tab = A;
        int n = b.size();

        for (int i = 0; i < n; i++) {
            assert(tab[i].size() == size_t(n));
            tab[i].push_back(b[i]);
        }

        int r = 0;
        for (int j = 0; j < n; j++) {
            for (int i = r; i < n; i++) {
                if (tab[i][j] != 0L) {
                    swap(tab[r], tab[i]);
                    goto reduce_row;
                }
            }
            continue;
        reduce_row:
            for (int k = n; k >= j; k--) {
                tab[r][k] /= tab[r][j];
            }
            for (int i = r + 1; i < n; i++) {
                for (int k = n; k >= j; k--) {
                    tab[i][k] -= tab[i][j] * tab[r][k];
                }
            }
            r++;
        }

        for (int i = r; i < n; i++) {
            if (tab[i][n] != 0L) {
                return IMPOSSIBLE;
            }
        }
        if (r < n) {
            return DEGENERATE;
        }

        for (int j = n - 1; j >= 0; j--) {
            for (int i = 0; i < j; i++) {
                tab[i][n] -= tab[i][j] * tab[j][n];
            }
        }

        x.resize(n, 0);
        for (int i = 0; i < n; i++) {
            x[i] = tab[i][n];
        }

        return SOLVED;
    }
};

struct inverse {
    vector<vector<frac>> A;

    // fill in the matrix...

    vector<vector<frac>> B;

    bool compute() {
        vector<vector<frac>> tab = A;
        int n = A.size();
        B.resize(n);

        for (int i = 0; i < n; i++) {
            assert(A[i].size() == size_t(n));
            B[i].assign(n, 0);
            B[i][i] = 1;
        }

        for (int j = 0; j < n; j++) {
            for (int i = j; i < n; i++) {
                if (tab[i][j] != 0L) {
                    swap(tab[i], tab[j]);
                    goto reduce_row;
                }
            }
            return false;
        reduce_row:
            for (int k = 0; k < n; k++) {
                B[j][k] /= tab[j][j];
            }
            for (int k = n - 1; k >= j; k--) {
                tab[j][k] /= tab[j][j];
            }
            for (int i = j + 1; i < n; i++) {
                for (int k = 0; k < n; k++) {
                    B[i][k] -= tab[i][j] * B[j][k];
                }
                for (int k = n - 1; k >= j; k--) {
                    tab[i][k] -= tab[i][j] * tab[j][k];
                }
            }
        }

        for (int j = n - 1; j >= 0; j--) {
            for (int i = 0; i < j; i++) {
                for (int k = 0; k < n; k++) {
                    B[i][k] -= tab[i][j] * B[j][k];
                }
            }
        }

        return true;
    }
};
