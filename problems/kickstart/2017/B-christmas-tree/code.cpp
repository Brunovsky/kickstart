#include <bits/stdc++.h>

using namespace std;

#define chmax(store, value) store = max(store, value)

// *****

int R, C, K;
vector<string> grid;

// dp[r][c][k]: maxsize of k-christmas tree seated on (r,c)
int dp[101][101][101];
// hi[r][c]: height of the tree rooted on (r,c)
int hi[101][101];

#define GREEN(r, c) (grid[r - 1][c - 1] == '#')
#define GREEN2(r, c) (GREEN(r, c - 1) && GREEN(r, c))

auto solve() {
    cin >> R >> C >> K >> ws;
    grid.resize(R);
    for (int i = 0; i < R; ++i) {
        getline(cin, grid[i]);
    }

    memset(dp, 0, sizeof(dp));
    memset(hi, 0, sizeof(hi));

    // compute heights
    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (GREEN(r, c)) {
                hi[r][c] = 1;

                int n = 1;
                int c2 = c + 2;
                while (n < r && c2 <= C && GREEN2(r, c2) && hi[r - n][c + n] == n) {
                    hi[r - n][c + n]++;
                    n++, c2 += 2;
                }
            }
        }
    }

    for (int r = 1; r <= R; r++) {
        int p = 1;
        do {
            while (p <= C && !GREEN(r, p)) {
                p++;
            }
            if (p > C) {
                break;
            }
            int cmin = p++;
            while (p <= C && GREEN(r, p)) {
                p++;
            }
            int cmax = p - 1;

            // the range [cmin,cmax] is all greens, surrounded by dots
            for (int c1 = cmin; c1 <= cmax; c1++) {
                int c2 = cmax - ((cmax - c1) & 1);
                int n = (c2 - c1) / 2; // height of the triangle above, n >= 0

                // S[k]: largest k-christmas tree... so far
                int S[101] = {0};

                // the range is too wide for the good triangle to fit
                if (n >= r) {
                    n = r - 1;
                    c2 = c1 + 2 * n;
                }

                // keep trimming until we find an actual good triangle
                while (n >= 0 && hi[r - n][c1 + n] <= n) {
                    n--, c2 -= 2;
                }

                while (n >= 0) {
                    // above the root of the good triangle
                    const auto &above = dp[r - (n + 1)][c1 + n];
                    auto &L = dp[r][c2 - 1];
                    auto &R = dp[r][c2];

                    int kth_triangle = (n + 1) * (n + 1);

                    for (int k = 1; k <= K && (k == 1 || above[k - 1]); k++) {
                        chmax(S[k], above[k - 1] + kth_triangle);
                    }

                    // n == 0 means L is spurious
                    if (n > 0) {
                        for (int k = 1; k <= K; k++) {
                            chmax(L[k], S[k]);
                        }
                    }

                    for (int k = 1; k <= K; k++) {
                        chmax(R[k], S[k]);
                    }

                    n--, c2 -= 2;
                }
            }
        } while (p <= C);
    }

    int best = 0;
    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (GREEN(r, c)) {
                chmax(best, dp[r][c][K]);
            }
        }
    }
    return best;
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
