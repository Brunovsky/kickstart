#include <bits/stdc++.h>

using namespace std;

// *****

#define MAX ('Z' - 'A' + 1)

auto solve() {
    int R, C;
    cin >> R >> C >> ws;

    vector<vector<int>> board(R, vector<int>(C, 0));
    set<int> found;

    for (int r = 0; r < R; ++r) {
        string line;
        cin >> line >> ws;
        for (int c = 0; c < C; ++c) {
            // use 0..25 instead of A..Z
            int n = line[c] - 'A';
            board[R - r - 1][c] = n;
            found.insert(n);
        }
    }

    vector<set<int>> in_adj(MAX);
    vector<set<int>> out_adj(MAX);

    for (int r = 1; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int upper = board[r][c];
            int lower = board[r - 1][c];
            if (upper == lower)
                continue;
            in_adj[upper].insert(lower);
            out_adj[lower].insert(upper);
        }
    }

    auto find_poly = [&]() {
        for (int poly : found)
            if (in_adj[poly].empty())
                return poly;
        return -1;
    };

    string answer;

    while (!found.empty()) {
        int lower = find_poly();
        if (lower == -1)
            return "-1"s;

        found.erase(lower);

        for (int upper : out_adj[lower])
            in_adj[upper].erase(lower);

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
