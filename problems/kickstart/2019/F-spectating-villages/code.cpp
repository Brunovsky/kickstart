#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXV 100'001

int V;
vector<vector<int>> adj; // neighbours, both parent and children

long B[MAXV];       // beauty
long parent[MAXV];  // LH on parent -> u illuminated
long current[MAXV]; // LH on u -> u + children illuminated
long none[MAXV];    // no LH on u or children -> u not illuminated
long exists[MAXV];  // no LH on u, LH on at least 1 children -> u illuminated

void visit(int u, int ancestor) {
    parent[u] = B[u];
    current[u] = B[u];
    none[u] = 0;
    exists[u] = B[u];

    long lighthouse = LONG_MIN;
    int count = 0;

    for (int v : adj[u]) {
        if (v == ancestor) {
            continue;
        }
        visit(v, u);
        count++;

        long cell = max(current[v], max(none[v], exists[v]));
        lighthouse = max(lighthouse, current[v] - cell);

        parent[u] += cell;
        current[u] += max(current[v], parent[v]);
        none[u] += max(none[v], exists[v]);
        exists[u] += cell;
    }

    if (count) {
        exists[u] += lighthouse;
    } else {
        exists[u] = 0;
    }
}

auto solve() {
    cin >> V >> ws;
    adj.assign(V + 1, {});
    for (int i = 1; i <= V; i++) {
        cin >> B[i] >> ws;
    }
    for (int i = 0; i < V - 1; i++) {
        int u, v;
        cin >> u >> v >> ws;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    visit(1, 1);
    return max(current[1], max(none[1], exists[1]));
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
