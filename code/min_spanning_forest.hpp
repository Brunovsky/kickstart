#ifndef MIN_SPANNING_FOREST_HPP
#define MIN_SPANNING_FOREST_HPP

#include "disjoint_set.hpp" // disjoint_set
#include "graph.hpp"

// *****

long min_spanning_forest_kruskal(int V, const edges_t& g, const vector<long>& weight) {
    int E = g.size();
    disjoint_set set(V);
    vector<int> edges(E);
    iota(begin(edges), end(edges), 0);
    sort(begin(edges), end(edges),
         [&weight](int e1, int e2) { return weight[e1] < weight[e2]; });

    long msf = 0;
    for (int e : edges) {
        auto [u, v] = g[e];
        if (set.find(u) != set.find(v)) {
            msf += weight[e];
            set.join(u, v);
        }
    }
    return msf;
}

long min_spanning_forest_prim(int V, const edges_t& g, const vector<long>& weight) {
    int E = g.size(), j = 0;
    vector<int> adj(2 * E);
    vector<int> off(V + 1, 0);
    for (auto [u, v] : g)
        off[u + 1]++, off[v + 1]++;
    partial_sum(begin(off), end(off), begin(off));
    auto cur = off;
    for (auto [u, v] : g)
        adj[cur[u]++] = adj[cur[v]++] = j++;

    vector<bool> vis(V, false);
    long msf = 0;
    for (int n = 0; n < V; n++) {
        if (vis[n])
            continue;

        priority_queue<pair<long, int>> Q;
        Q.push({0, n});

        while (!Q.empty()) {
            auto [nw, u] = Q.top();
            Q.pop();
            if (vis[u])
                continue;

            vis[u] = true;
            msf += -nw;
            for (int i = off[u]; i < off[u + 1]; i++)
                if (int e = adj[i], v = g[e][u != g[e][1]]; !vis[v])
                    Q.push({-weight[e], v});
        }
    }
    return msf;
}

#endif // MIN_SPANNING_FOREST_HPP
