#include <bits/stdc++.h>

using namespace std;

// *****

int N;
vector<vector<int>> adj;

constexpr int8_t CLEAN = 0, ENTER = 1, EXIT = 2, CYCLE = 3;

vector<int8_t> visited;
vector<int> dist;
int cycle_root;

auto enter_cycle(int i, int p) {
    // Detected a cycle?
    if (visited[i] == ENTER) {
        cycle_root = i;
        return CYCLE;
    }

    assert(visited[i] == CLEAN);
    visited[i] = ENTER;

    for (int j : adj[i]) {
        if (j == p) {
            continue;
        }
        auto command = enter_cycle(j, i);
        if (command == CYCLE) {
            visited[i] = CLEAN;
            dist[i] = 0;
            return (i == cycle_root) ? EXIT : CYCLE;
        } else if (command == EXIT) {
            visited[i] = CLEAN;
            return EXIT;
        }
    }

    visited[i] = CLEAN;
    return CLEAN;
}

void enter_dist(int i, int p) {
    if (visited[i] != CLEAN) {
        return;
    }
    visited[i] = ENTER;

    for (int j : adj[i]) {
        if (j != p && visited[j] == CLEAN) {
            dist[j] = min(dist[i] + 1, dist[j]);
            enter_dist(j, i);
        }
    }
}

void solve() {
    cin >> N >> ws;
    adj.assign(N, {});
    for (int n = 0; n < N; ++n) {
        int xi, yi;
        cin >> xi >> yi >> ws;
        --xi, --yi;
        adj[xi].push_back(yi);
        adj[yi].push_back(xi);
    }

    visited.assign(N, CLEAN);
    dist.assign(N, N);

    enter_cycle(0, 0);

    for (int i = 0; i < N; ++i) {
        if (dist[i] == 0) {
            enter_dist(i, i);
            break;
        }
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
        cout << "Case #" << t << ":";
        for (int d : dist) {
            cout << ' ' << d;
        }
        cout << '\n';
    }
    return 0;
}
