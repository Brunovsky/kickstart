#ifndef ISOMORPHISM_HPP
#define ISOMORPHISM_HPP

#include "graph.hpp"
#include "graph_operations.hpp"
#include "hash.hpp"
#include "matrix.hpp"

// *****

/**
 * Complexity: O(V^3 log(V)) time
 *             O(V^2) memory
 */
auto hash_graph_vertices(int V, const edges_t& g) {
    static vec_hasher hasher;
    mat<size_t> m(V, V);
    for (auto [u, v] : g)
        m[u][v] = m[v][u] = 1;
    m = m ^ V;

    vector<size_t> hashtable(V);
    for (int n = 0; n < V; n++) {
        sort(begin(m[n]), end(m[n]));
        hashtable[n] = hasher(m[n]);
    }
    return hashtable;
}

/**
 * Compute the topological hash of a graph, irrespective of its labels.
 */
size_t hash_graph(int V, const edges_t& g) {
    static vec_hasher hasher;
    auto hashtable = hash_graph_vertices(V, g);
    sort(begin(hashtable), end(hashtable));
    hashtable.push_back(V), hashtable.push_back(g.size());
    return hasher(hashtable);
}

/**
 * Isomorphism heuristic for two graphs
 */
bool isomorphic(int V, const edges_t& g1, const edges_t& g2) {
    return g1.size() == g2.size() && hash_graph(V, g1) == hash_graph(V, g2);
}

#endif // ISOMORPHISM_HPP
