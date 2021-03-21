#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "hash.hpp"

// *****

using edge_t = array<int, 2>;
using edges_t = vector<edge_t>;
using edgeset_t = unordered_set<edge_t, pair_hasher>;
using offsets_t = vector<int>;
using adjacency_t = vector<int>;
using degrees_t = vector<int>;
using ranks_t = vector<int>;
using parent_t = vector<int>;
using adjacency_lists_t = vector<adjacency_t>;
using dist_t = long;
using flow_t = long;
using cost_t = long;
using cap_t = long;
using dists_t = vector<long>;
using flows_t = vector<long>;
using costs_t = vector<long>;
using caps_t = vector<long>;

/**
 * Edgelist+offset based graph constructor (template)
 */
struct edgelist_offset_template {
    int V, E = 0;
    vector<int> adj, off;

    edgelist_offset_template(int V, const edges_t& g)
        : V(V), E(g.size()), adj(2 * E), off(V + 1) {
        for (auto [u, v] : g)
            off[u + 1]++, off[v + 1]++;
        inclusive_scan(begin(off), end(off), begin(off));
        vector<int> cur = off;
        for (auto [u, v] : g)
            adj[cur[u]++] = v, adj[cur[v]++] = u;
    }

    inline int len(int u) const { return off[u + 1] - off[u]; }
};

/**
 * Adjacency list based graph constructor (template)
 */
struct adjacency_list_template {
    int V, E = 0;
    vector<vector<int>> adj;

    adjacency_list_template(int V, const edges_t& g) : V(V), E(g.size()), adj(V) {
        for (auto [u, v] : g)
            adj[u].push_back(v), adj[v].push_back(u);
    }

    inline int len(int u) const { return adj[u].size(); }
};

#endif // GRAPH_HPP
