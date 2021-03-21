#include <bits/stdc++.h>

using namespace std;

// *****
#define MAXN 302

int R, C;
int H[MAXN][MAXN];

int max_adjacent(int r, int c) {
    return max(max(H[r + 1][c], H[r - 1][c]), max(H[r][c - 1], H[r][c + 1]));
}
int max_all(int r, int c) { return max(H[r][c], max_adjacent(r, c)); }

using pos_t = array<int, 2>;

constexpr array<int, 2> dd[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

auto solve() {
    cin >> R >> C;
    memset(H, 0, sizeof(H));

    using entry_t = pair<int, pos_t>;
    priority_queue<entry_t> Q;
    for (int r = 1; r <= R; r++)
        for (int c = 1; c <= C; c++)
            cin >> H[r][c];
    for (int r = 1; r <= R; r++)
        for (int c = 1; c <= C; c++)
            Q.push({max_all(r, c), {r, c}});

    long added = 0;

    while (!Q.empty()) {
        auto [h, pos] = Q.top();
        auto [r, c] = pos;
        Q.pop();
        if (H[r][c] > h)
            continue;

        int needed = max(max_adjacent(r, c) - 1 - H[r][c], 0);
        if (needed > 0) {
            added += needed;
            h = H[r][c] + needed;
            for (auto [dr, dc] : dd) {
                int rx = r + dr, cx = c + dc;
                if (rx > 0 && rx <= R && cx > 0 && cx <= C && max_all(rx, cx) < h)
                    Q.push({h, {rx, cx}});
            }
            H[r][c] = h;
        }
    }

    return added;
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
