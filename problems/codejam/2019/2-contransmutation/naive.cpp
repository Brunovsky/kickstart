#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXM 20
constexpr long mod = 1000000007;

int M;
long G[MAXM];
vector<array<int, 2>> adj;

void produce(int u, int amt = 0) {
    amt = amt == 0 ? G[u] : amt;
    for (int v : adj[u])
        G[v] += amt;
    G[u] -= amt;
}

void simulate(int iterations) {
    for (int k = 0; k < iterations; k++) {
        for (int u = 1; u < M; u++) {
            produce(u);
        }
    }
}

auto solve() {
    cin >> M;
    adj.assign(M, {});
    for (int u = 0; u < M; u++) {
        int v, w;
        cin >> v >> w;
        v--, w--;
        adj[u] = {v, w};
    }
    for (int u = 0; u < M; u++) {
        cin >> G[u];
    }
    const long max = *max_element(G, G + M);
    const long INF = (1 << max) * 10 * M + 1;

    while (G[0] <= INF) {
        long lead = G[0];
        simulate(M + 1);
        if (lead == G[0])
            break;
    }
    if (G[0] > INF) {
        return "UNBOUNDED"s;
    }
    if (G[0] == 0) {
        return "0"s;
    }
    long lead = G[0];
    produce(0, 1);
    simulate(M + 1);
    if (G[0] <= lead) {
        return to_string(lead);
    } else {
        return "UNBOUNDED"s;
    }
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
