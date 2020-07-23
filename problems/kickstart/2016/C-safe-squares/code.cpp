#include <bits/stdc++.h>

using namespace std;

// *****

int grid[3001][3001];

auto solve() {
    int R, C, K;
    cin >> R >> C >> K >> ws;
    memset(grid, 0, sizeof(grid));
    for (int k = 0; k < K; ++k) {
        int r, c;
        cin >> r >> c >> ws;
        grid[r + 1][c + 1] = 1;
    }
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            grid[r][c] += grid[r][c - 1] + grid[r - 1][c] - grid[r - 1][c - 1];
        }
    }
    long count = R * C - K;
    for (int len = 2, most = min(R, C); len <= most; ++len) {
        for (int r = len; r <= R; ++r) {
            for (int c = len; c <= C; ++c) {
                int nw = grid[r - len][c - len];
                int ne = grid[r - len][c];
                int sw = grid[r][c - len];
                int se = grid[r][c];
                int monsters = se - (sw + ne) + nw;
                count += monsters == 0;
            }
        }
    }
    return count;
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
