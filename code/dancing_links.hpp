#include <bits/stdc++.h>

using namespace std;

// *****

using int2 = array<int, 2>;

// 2d dancing links, non-wrapping
struct dancing_links_matrix {
    int n, m;
    vector<vector<int>> S, N, E, W;

    dancing_links_matrix(int n, int m) : n(n), m(m) {
        N.assign(n + 2, vector<int>(m + 2));
        S.assign(n + 2, vector<int>(m + 2));
        E.assign(n + 2, vector<int>(m + 2));
        W.assign(n + 2, vector<int>(m + 2));
        for (int i = 0; i <= n + 1; i++) {
            for (int j = 0; j <= m + 1; j++) {
                S[i][j] = i + 1;
                N[i][j] = i - 1;
                E[i][j] = i + 1;
                W[i][j] = i - 1;
            }
        }
    }

    void insert(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = c;
        W[r][E[r][c]] = c;
        S[N[r][c]][c] = r;
        N[S[r][c]][c] = r;
    }

    void remove(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = E[r][c];
        W[r][E[r][c]] = W[r][c];
        S[N[r][c]][c] = S[r][c];
        N[S[r][c]][c] = N[r][c];
    }

    int2 south(int2 p) const {
        int r = p[0], c = p[1];
        return {S[r][c], c};
    }

    int2 north(int2 p) const {
        int r = p[0], c = p[1];
        return {N[r][c], c};
    }

    int2 east(int2 p) const {
        int r = p[0], c = p[1];
        return {r, E[r][c]};
    }

    int2 west(int2 p) const {
        int r = p[0], c = p[1];
        return {r, W[r][c]};
    }
};
