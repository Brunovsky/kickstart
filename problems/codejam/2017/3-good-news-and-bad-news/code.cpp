#include <bits/stdc++.h>

using namespace std;

// *****

default_random_engine mt(random_device{}());
using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

int V, E, depth;
vector<vector<int>> adj;
vector<array<int, 2>> edgelist;
vector<int> sum, flow;
vector<int> label, lo, hi;
intd rng_flow;
boold coin(0.5);

int rand_flow() { return coin(mt) ? rng_flow(mt) : -rng_flow(mt); }

bool bridge_dfs(int u, int p = -1) {
    label[u] = lo[u] = hi[u] = depth++;

    for (int e : adj[u]) {
        bool uv = u == edgelist[e][0];
        int v = edgelist[e][uv];

        if (!label[v]) {
            if (bridge_dfs(v, e))
                return true;

            lo[u] = min(lo[u], lo[v]);
            hi[u] = max(hi[u], hi[v]);
        } else if (e != p) {
            lo[u] = min(lo[u], label[v]);
            hi[u] = max(hi[u], label[v]);
        }
    }

    return p != -1 && lo[u] == label[u] && hi[u] < depth;
}

void dfs(int u, int p = -1) {
    label[u] = depth++;

    for (int e : adj[u]) {
        bool uv = u == edgelist[e][0];
        int v = edgelist[e][uv];

        if (label[v] && label[u] > label[v] && e != p) {
            flow[e] = rand_flow();
            sum[u] += uv ? -flow[e] : flow[e];
            sum[v] += uv ? flow[e] : -flow[e];
        } else if (!label[v]) {
            dfs(v, e);
            flow[e] = uv ? -sum[v] : sum[v];
            sum[u] += uv ? -flow[e] : flow[e];
            sum[v] += uv ? flow[e] : -flow[e];
        }
    }
}

auto solve() {
    cin >> V >> E;
    adj.assign(V, {});
    edgelist.resize(E);
    for (int e = 0; e < E; e++) {
        auto& [u, v] = edgelist[e];
        cin >> u >> v, u--, v--;
        adj[u].push_back(e);
        adj[v].push_back(e);
    }

    label.assign(V, 0);
    lo.assign(V, 0);
    hi.assign(V, 0);
    depth = 1;
    for (int u = 0; u < V; u++) {
        if (!label[u] && bridge_dfs(u)) {
            cout << " IMPOSSIBLE\n";
            return;
        }
    }

    int fmin = -V * V, fmax = V * V;
    rng_flow = V <= 20 ? intd(1, V) : intd(1 + V / 4, V);
    bool ok;

    do {
        label.assign(V, 0);
        sum.assign(V, 0);
        flow.assign(E, 0);
        depth = 1;

        for (int u = 0; u < V; u++)
            if (!label[u])
                dfs(u);

        ok = true;
        for (int e = 0; e < E && ok; e++)
            ok &= flow[e] != 0 && fmin <= flow[e] && flow[e] <= fmax;
    } while (!ok);

    for (int f : flow) {
        cout << ' ' << f;
    }
    cout << "\n";
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
    }
    return 0;
}
