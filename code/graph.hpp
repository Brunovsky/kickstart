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

/**
 * Edgelist+offset based graph constructor (template)
 */
struct edgelist_offset_t {
    int V, E = 0;
    vector<int> adj, off;

    edgelist_offset_t(int V, const edges_t& g)
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

#endif // GRAPH_HPP
