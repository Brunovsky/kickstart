#include <bits/stdc++.h>

using namespace std;

// *****

int R, C;
vector<string> grid;

auto floodfill(array<array<char, 2>, 2> w, int rq, int cq) {
    static const vector<array<int, 2>> dd = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    vector<vector<bool>> vis(R, vector<bool>(C, false));
    int best = 0;

    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (vis[r][c] || grid[r][c] != w[r >= rq][c >= cq])
                continue;

            queue<array<int, 2>> frontier;

            frontier.push({r, c});
            vis[r][c] = true;
            int conn = 1;

            while (!frontier.empty()) {
                auto [rm, cm] = frontier.front();
                frontier.pop();

                for (auto d : dd) {
                    int rn = rm + d[0], cn = cm + d[1];
                    if (0 <= rn && rn < R && 0 <= cn && cn < C && !vis[rn][cn] &&
                        grid[rn][cn] == w[rn >= rq][cn >= cq]) {
                        frontier.push({rn, cn});
                        vis[rn][cn] = true;
                        conn++;
                    }
                }
            }

            best = max(best, conn);
        }
    }

    return best;
}

auto solve() {
    cin >> R >> C >> ws;
    grid.assign(R, "");

    for (auto& row : grid)
        getline(cin, row);

    bool pattern[16] = {};

    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            pattern[15 * (grid[r][c] == 'B')] = true;
            if (r + 1 < R) {
                int n = grid[r][c] == 'B';
                int s = grid[r + 1][c] == 'B';
                pattern[12 * n + 3 * s] = true;
            }
            if (c + 1 < C) {
                int w = grid[r][c] == 'B';
                int e = grid[r][c + 1] == 'B';
                pattern[10 * w + 5 * e] = true;
            }
            if (r + 1 < R && c + 1 < C) {
                // find pattern index around current cross
                int nw = grid[r][c] == 'B';
                int ne = grid[r][c + 1] == 'B';
                int sw = grid[r + 1][c] == 'B';
                int se = grid[r + 1][c + 1] == 'B';
                pattern[8 * nw + 4 * ne + 2 * sw + se] = true;
            }
        }
    }

    int best = 0;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            for (int i = 0; i < 16; i++) {
                if (pattern[i]) {
                    char nw = (i & 8) ? 'B' : 'W';
                    char ne = (i & 4) ? 'B' : 'W';
                    char sw = (i & 2) ? 'B' : 'W';
                    char se = (i & 1) ? 'B' : 'W';
                    best = max(best, floodfill({{{nw, ne}, {sw, se}}}, r, c));
                }
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
