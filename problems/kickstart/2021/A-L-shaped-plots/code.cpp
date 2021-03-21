#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 1002

int R, C;
int A[MAXN][MAXN];
int north[MAXN][MAXN], south[MAXN][MAXN];
int west[MAXN][MAXN], east[MAXN][MAXN];

auto solve() {
    cin >> R >> C;
    memset(A, 0, sizeof(A));
    memset(north, 0, sizeof(north));
    memset(south, 0, sizeof(south));
    memset(west, 0, sizeof(west));
    memset(east, 0, sizeof(east));

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            cin >> A[r][c];
        }
    }

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (A[r][c]) {
                north[r][c] = 1 + north[r - 1][c];
                west[r][c] = 1 + west[r][c - 1];
            }
        }
    }

    for (int r = R; r >= 1; r--) {
        for (int c = C; c >= 1; c--) {
            if (A[r][c]) {
                south[r][c] = 1 + south[r + 1][c];
                east[r][c] = 1 + east[r][c + 1];
            }
        }
    }

    long total = 0;

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (A[r][c]) {
                int n = north[r][c], s = south[r][c], w = west[r][c], e = east[r][c];
                total += min((n - 2) / 2, w - 1);
                total += min((n - 2) / 2, e - 1);
                total += min((s - 2) / 2, w - 1);
                total += min((s - 2) / 2, e - 1);
                total += min((w - 2) / 2, n - 1);
                total += min((w - 2) / 2, s - 1);
                total += min((e - 2) / 2, n - 1);
                total += min((e - 2) / 2, s - 1);
            }
        }
    }

    return total;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
