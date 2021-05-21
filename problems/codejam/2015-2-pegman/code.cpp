#include <bits/stdc++.h>

using namespace std;

// *****

#define DIR(c)          (c == '^' ? 1 : c == 'v' ? 2 : c == '<' ? 3 : c == '>' ? 4 : 0)
#define DIRECTION(r, c) DIR(grid[r][c])

constexpr int dd[5][2] = {{}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}};

auto solve() {
    int R, C;
    cin >> R >> C >> ws;
    vector<string> grid(R + 2);
    vector<int> onrow(R + 2, 0);
    vector<int> oncol(C + 2, 0);

    for (int r = 1; r <= R; r++) {
        string line;
        cin >> line;
        grid[r] = "#" + line + "#";
        for (int c = 1; c <= C; c++)
            if (DIRECTION(r, c))
                onrow[r]++, oncol[c]++;
    }

    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (DIRECTION(r, c) && onrow[r] == 1 && oncol[c] == 1)
                return "IMPOSSIBLE"s;
        }
    }

    int flips = 0;
    for (int r = 1; r <= R; r++) {
        for (int c = 1; c <= C; c++) {
            if (int d = DIRECTION(r, c); d) {
                int dr = r + dd[d][0], dc = c + dd[d][1];
                bool ok = false;
                while (!ok && 1 <= dr && dr <= R && 1 <= dc && dc <= C) {
                    ok = DIRECTION(dr, dc);
                    dr += dd[d][0], dc += dd[d][1];
                }
                flips += !ok;
            }
        }
    }
    return to_string(flips);
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
