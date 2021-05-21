#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXR 101
#define MAXC 101
#define CHIP '@'

int R, C, H, V;
string grid[MAXR];
int cnt[MAXR][MAXC];

auto solve() {
    cin >> R >> C >> H >> V;
    for (int i = 0; i < R; i++) {
        cin >> grid[i];
    }
    memset(cnt, 0, sizeof(cnt));
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            cnt[i + 1][j + 1] = grid[i][j] == CHIP;
        }
    }
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            cnt[i + 1][j + 1] += cnt[i + 1][j] + cnt[i][j + 1] - cnt[i][j];
        }
    }
    int total = cnt[R][C];
    int each = total / ((V + 1) * (H + 1));
    int remaining = total % ((V + 1) * (H + 1));
    if (remaining != 0) {
        return "IMPOSSIBLE"s;
    }
    if (total == 0) {
        return "POSSIBLE"s;
    }
    int each_R = (V + 1) * each;
    int each_C = (H + 1) * each;
    int need_R = each_R;
    int need_C = each_C;
    vector<int> rs{0}, cs{0};
    for (int r = 1; r <= R; r++) {
        int chips = cnt[r][C];
        if (chips == need_R) {
            need_R += each_R;
            rs.push_back(r);
        } else if (chips > need_R) {
            return "IMPOSSIBLE"s;
        }
    }
    for (int c = 1; c <= C; c++) {
        int chips = cnt[R][c];
        if (chips == need_C) {
            need_C += each_C;
            cs.push_back(c);
        } else if (chips > need_C) {
            return "IMPOSSIBLE"s;
        }
    }
    assert(int(rs.size()) == H + 2 && int(cs.size()) == V + 2);
    // (r1,c1)
    //          (r2,c2)
    for (int i = 0; i < H + 1; i++) {
        int r1 = rs[i], r2 = rs[i + 1];
        for (int j = 0; j < V; j++) {
            int c1 = cs[j], c2 = cs[j + 1];
            int chips = cnt[r2][c2] - cnt[r2][c1] - cnt[r1][c2] + cnt[r1][c1];
            if (chips != each) {
                return "IMPOSSIBLE"s;
            }
        }
    }
    return "POSSIBLE"s;
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
