#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 26
#define MAXS 30

int board[MAXS][MAXS];

auto solve() {
    int R, C;
    cin >> R >> C >> ws;

    memset(board, 0, sizeof(board));
    set<int> found;

    for (int r = 0; r < R; ++r) {
        string line;
        cin >> line >> ws;
        for (int c = 0; c < C; ++c) {
            int n = line[c] - 'A';
            board[R - r - 1][c] = n;
            found.insert(n);
        }
    }

    vector<set<int>> in_adj(MAXN);
    vector<set<int>> out_adj(MAXN);

    for (int r = 1; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int upper = board[r][c];
            int lower = board[r - 1][c];
            if (upper != lower) {
                in_adj[upper].insert(lower);
                out_adj[lower].insert(upper);
            }
        }
    }

    string answer;

    while (!found.empty()) {
        int lower = -1;
        for (int poly : found) {
            if (in_adj[poly].empty()) {
                lower = poly;
                break;
            }
        }
        if (lower == -1) {
            return "-1"s;
        }

        found.erase(lower);

        for (int upper : out_adj[lower]) {
            in_adj[upper].erase(lower);
        }

        answer += char(lower + 'A');
    }

    return answer;
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
