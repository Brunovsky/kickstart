#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long mod = 1000000007;

struct strongly_connected_components {
    // vertex graph
    int V;
    vector<array<int, 2>> adjout;
    vector<vector<int>> adjin;
    vector<int> G;

    // scc graph
    int C;
    vector<vector<int>> cadjout, cadjin;
    vector<vector<int>> components;
    vector<int> csize;
    vector<int> cmap;
    vector<int> topo;
    vector<long> fuel;
    vector<bool> unbounded;
    vector<bool> source;

    vector<int> index;
    vector<int> lowlink;
    vector<bool> vis;
    stack<int> S;
    int depth;

    strongly_connected_components(int V) : V(V) {
        adjout.assign(V, {});
        adjin.assign(V, {});
        G.assign(V, 0);
    }

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && 0 <= w && w < V);
        assert(v != w);
        adjout[u][0] = v;
        adjout[u][1] = w;
        adjin[v].push_back(u);
        adjin[w].push_back(u);
    }

    void dfs(int u) {
        index[u] = depth;
        lowlink[u] = depth;
        depth++;
        S.push(u);
        vis[u] = true;

        for (int v : adjout[u]) {
            if (!index[v]) {
                dfs(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (vis[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            vector<int> component;
            int v;
            do {
                v = S.top();
                S.pop();
                vis[v] = false;
                component.push_back(v);
            } while (u != v);
            components.push_back(move(component));
        }
    }

    void compute_scc() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        vis.assign(V, false);
        S = stack<int>();
        depth = 1;
        components.clear();

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                dfs(u);
            }
        }
        C = components.size();
    }

    void compute_scc_graph() {
        cmap.assign(V, 0);
        csize.assign(V, 0);
        for (int a = 0; a < C; a++) {
            for (int u : components[a]) {
                cmap[u] = a;
            }
            csize[a] = components[a].size();
        }
        cadjout.assign(C, {});
        cadjin.assign(C, {});
        for (int a = 0; a < C; a++) {
            unordered_set<int> out;
            for (int u : components[a]) {
                for (int v : adjout[u]) {
                    if (a != cmap[v]) {
                        out.insert(cmap[v]);
                    }
                }
            }
            for (int b : out) {
                cadjout[a].push_back(b);
                cadjin[b].push_back(a);
            }
        }
    }

    void compute_topo_sort() {
        queue<int> frontier;
        vector<int> cnt(C, 0);
        for (int a = 0; a < C; a++) {
            cnt[a] = cadjin[a].size();
            if (cnt[a] == 0) {
                frontier.push(a);
            }
        }
        while (!frontier.empty()) {
            int a = frontier.front();
            frontier.pop();
            topo.push_back(a);
            for (int b : cadjout[a]) {
                if (--cnt[b] == 0) {
                    frontier.push(b);
                }
            }
        }
    }

    bool self_feeding(int a) {
        for (int u : components[a]) {
            int v = adjout[u][0], w = adjout[u][1];
            if (cmap[v] == a && cmap[w] == a)
                return true;
        }
        return false;
    }

    bool has_loop(int a) {
        if (csize[a] > 1)
            return true;
        int u = components[a][0];
        return adjout[u][0] == u || adjout[u][1] == u;
    }

    void compute_unbounded_loops() {
        fuel.assign(C, 0);
        for (int u = 0; u < V; u++) {
            fuel[cmap[u]] += G[u];
        }

        source.assign(C, false);
        unbounded.assign(C, false);
        for (int a : topo) {
            source[a] = source[a] || (fuel[a] > 0);
            unbounded[a] = unbounded[a] || (source[a] && self_feeding(a));
            for (int b : cadjout[a]) {
                source[b] = source[b] || source[a];
                unbounded[b] = unbounded[b] || unbounded[a] || (source[a] && has_loop(a));
            }
        }
    }

    long compute_lead() {
        vector<bool> feeder(C, false);
        queue<int> frontier;
        feeder[cmap[0]] = true;
        frontier.push(cmap[0]);

        while (!frontier.empty()) {
            int b = frontier.front();
            frontier.pop();
            for (int a : cadjin[b]) {
                if (!feeder[a] && source[a]) {
                    feeder[a] = true;
                    frontier.push(a);
                }
            }
        }
        vector<int> new_topo;
        for (int a : topo) {
            if (feeder[a]) {
                new_topo.push_back(a);
            }
        }
        reverse(begin(new_topo), end(new_topo));
        swap(new_topo, topo);

        vector<long> paths(C, 0);
        paths[cmap[0]] = 1;
        int T = topo.size();
        for (int i = 1; i < T; i++) {
            int a = topo[i];
            for (int u : components[a]) {
                for (int v : adjout[u]) {
                    if (feeder[cmap[v]]) {
                        paths[a] = (paths[a] + paths[cmap[v]]) % mod;
                    }
                }
            }
        }

        long lead = 0;
        for (int a : topo) {
            lead = (lead + paths[a] * (fuel[a] % mod)) % mod;
        }
        return lead;
    }

    auto solve() {
        compute_scc();
        compute_scc_graph();
        compute_topo_sort();
        compute_unbounded_loops();

        if (unbounded[cmap[0]]) {
            return "UNBOUNDED"s;
        }

        long lead = compute_lead();
        return to_string(lead);
    }
};

auto solve() {
    int M;
    cin >> M;
    strongly_connected_components scc(M);
    for (int u = 0; u < M; u++) {
        int v, w;
        cin >> v >> w;
        v--, w--;
        scc.add(u, v, w);
    }
    for (int u = 0; u < M; u++) {
        cin >> scc.G[u];
    }
    return scc.solve();
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
