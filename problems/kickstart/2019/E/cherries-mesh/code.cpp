#include <bits/stdc++.h>

using namespace std;

// *****

int N, M;
vector<vector<int>> adj;
vector<int8_t> tag;

void dfs(int i) {
    tag[i] = 1;
    for (int j : adj[i]) {
        if (!tag[j]) {
            dfs(j);
        }
    }
}

auto solve() {
    cin >> N >> M >> ws;
    adj.assign(N, {});
    tag.assign(N, 0);
    for (int m = 0; m < M; ++m) {
        int Ci, Di;
        cin >> Ci >> Di >> ws;
        --Ci, --Di;
        adj[Ci].push_back(Di);
        adj[Di].push_back(Ci);
    }

    int components = 0;

    for (int i = 0; i < N; ++i) {
        if (!tag[i]) {
            dfs(i);
            ++components;
        }
    }

    return (N - 1) + (components - 1);
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
