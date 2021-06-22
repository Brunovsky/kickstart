#include "../struct/y_combinator.hpp"
#include "../struct/disjoint_set.hpp"

/**
 * A repertoir of techniques on an undirected graph with values on nodes
 * Focus on bridges and biconnected components
 * Assumes graph is 1-indexed
 */
void undirected_graph_sample(int V, vector<array<int, 2>> edges) {
    int E = edges.size();

    vector<vector<pair<int, int>>> adj(V + 1);
    for (int e = 0; e < E; e++) {
        auto [u, v] = edges[e];
        adj[u].emplace_back(e, v);
        adj[v].emplace_back(e, u);
    }

    // ***** DFS tree
    vector<bool> back_edge(E);
    vector<bool> bridge_edge(E);
    vector<int> index(V + 1);
    vector<int> lowlink(V + 1);
    vector<int> bridges;
    int dfs_timer = 1;

    auto dfs = y_combinator([&](auto self, int u, int p) {
        index[u] = lowlink[u] = dfs_timer++;

        for (auto [e, v] : adj[u]) {
            if (v != p) {
                if (index[v]) {
                    edges[e] = {v, u}; // orientation
                    lowlink[u] = min(lowlink[u], index[v]);
                    back_edge[e] = true;
                } else {
                    edges[e] = {u, v}; // orientation
                    self(v, u);
                    lowlink[u] = min(lowlink[u], lowlink[v]);
                    if (lowlink[v] > index[u]) {
                        bridges.push_back(e);
                        bridge_edge[e] = true;
                    }
                }
            }
        }
    });

    for (int u = 1; u <= V; u++) {
        if (!index[u]) {
            dfs(u, 0);
        }
    }

    // ***** Biconnected components subgraph
    disjoint_set bcc_dsu(V + 1);
    vector<vector<pair<int, int>>> bcomp_adj(V + 1);

    for (int e = 0; e < E; e++) {
        if (!is_bridge[e]) {
            auto [u, v] = edges[e];
            bcc_dsu.join(u, v);
            bcomp_adj[u].emplace_back(e, v);
            bcomp_adj[v].emplace_back(e, u);
        }
    }

    vector<vector<int>> bcc_by_rep(V + 1);
    for (int u = 1; u <= V; u++) {
        bcc_by_rep[bcc_dsu.find(u)].push_back(u);
    }

    vector<vector<int>> bset;
    vector<int> bmap(V + 1, -1);
    for (int r = 1; r <= V; r++) {
        if (!bcc_by_rep[r].empty()) {
            int b = bset.size();
            for (int u : bcc_by_rep[r]) {
                bmap[u] = b;
            }
            bset.emplace_back(move(bcc_by_rep[r]));
        }
    }
    int B = bset.size();

    // ***** Biconnected components condensation
    vector<vector<pair<int, int>>> bcc_adj(B);

    for (int e : bridges) {
        auto [u, v] = bridges[e];
        assert(bmap[u] != bmap[v]);
        bcc_adj[bmap[u]].emplace_back(e, bmap[v]);
        bcc_adj[bmap[v]].emplace_back(e, bmap[u]);
    }
}
