#ifndef SCC_HPP
#define SCC_HPP

#include "../struct/y_combinator.hpp"
#include "../hash.hpp"

/**
 * Find strongly connected components in reverse topological order (Tarjan)
 * Nodes 0-indexed
 * Complexity: O(V + E), same for condensation
 */
auto build_scc(const vector<vector<int>>& adj) {
    int V = adj.size(), C = 0; // C = number of scc

    vector<vector<int>> cset;
    vector<int> cmap(V, -1);
    vector<int> index(V, 0);
    vector<int> lowlink(V, 0);
    int timer = 1;

    stack<int> S;

    auto dfs = y_combinator([&](auto self, int u) -> void {
        index[u] = lowlink[u] = timer++;
        S.push(u);

        for (int v : adj[u]) {
            if (index[v] && cmap[v] == -1) { // <-- skip cmap'd nodes
                lowlink[u] = min(lowlink[u], index[v]);
            } else if (!index[v]) {
                self(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            }
        }

        // found a strongly connected components
        if (index[u] == lowlink[u]) {
            int c = C++;
            cset.push_back({});
            int v;
            do {
                v = S.top(), S.pop();
                cmap[v] = c, cset[c].push_back(v);
            } while (u != v);
        }
    });

    for (int u = 0; u < V; u++) {
        if (!index[u]) {
            dfs(u);
        }
    }

    return make_pair(cset, cmap);
}

auto condensate_scc(const vector<vector<int>>& adj, const vector<int>& cmap) {
    int V = adj.size(), C = 0;
    unordered_set<array<int, 2>> edges;
    for (int u = 0; u < V; u++) {
        for (int v : adj[u]) {
            if (cmap[u] != cmap[v]) {
                edges.insert({cmap[u], cmap[v]});
            }
        }
        C = max(C, cmap[u] + 1);
    }

    vector<vector<int>> sccout(C), sccin(C);
    for (auto [u, v] : edges) {
        sccout[u].push_back(v);
        sccin[v].push_back(u);
    }
    return make_pair(sccout, sccin);
}

#endif // SCC_HPP
