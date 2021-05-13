#ifndef GRAPH_OPERATIONS_HPP
#define GRAPH_OPERATIONS_HPP

#include "../hash.hpp"
#include "../random.hpp"

/**
 * Generate a permutation pi of [0...V) and replace u with pi(u) for all u.
 */
auto relabel_inplace(int V, edges_t& g) {
    vector<int> label(V);
    iota(begin(label), end(label), 0);
    shuffle(begin(label), end(label), mt);
    for (auto& [u, v] : g)
        u = label[u], v = label[v];
}

auto relabel(int V, const edges_t& g) {
    auto h = g;
    return relabel_inplace(V, h), h;
}

auto relabel_inplace(int U, int V, edges_t& g) {
    vector<int> ulabel(U), vlabel(V);
    iota(begin(ulabel), end(ulabel), 0);
    iota(begin(vlabel), end(vlabel), 0);
    shuffle(begin(ulabel), end(ulabel), mt);
    shuffle(begin(vlabel), end(vlabel), mt);
    for (auto& [u, v] : g)
        u = ulabel[u], v = vlabel[v];
}

auto relabel(int U, int V, const edges_t& g) {
    auto h = g;
    return relabel_inplace(U, V, h), h;
}

/**
 * Randomly flip some edges with probability p
 */
auto random_flip_inplace(edges_t& g, double p = 0.5) {
    if (p <= 0.25) {
        for (int e : int_sample_p(p, 0, g.size()))
            swap(g[e][0], g[e][1]);
    } else {
        boold flipd(p);
        for (auto& [u, v] : g)
            if (flipd(mt))
                swap(u, v);
    }
}

auto random_flip(const edges_t& g, double p = 0.5) {
    auto h = g;
    return random_flip_inplace(h, p), h;
}

/**
 * Reverse the edges of the graph, and return a new graph.
 */
auto reverse(const edges_t& g) {
    edges_t h;
    h.reserve(g.size());
    for (auto [u, v] : g)
        h.push_back({v, u});
    return h;
}

/**
 * Join two graphs together.
 * The new graph has the two graphs joined as disconnected subgraphs.
 */
auto& join(edges_t& g, const edges_t& h) {
    g.insert(end(g), begin(h), end(h));
    return g;
}

auto make_adjacency_lists_undirected(const edges_t& g, int V) {
    vector<vector<int>> adj(V);
    for (auto [u, v] : g)
        adj[u].push_back(v), adj[v].push_back(u);
    return adj;
}

auto make_adjacency_lists_directed(const edges_t& g, int V) {
    vector<vector<int>> adj(V);
    for (auto [u, v] : g)
        adj[u].push_back(v);
    return adj;
}

auto make_adjacency_lists_reverse(const edges_t& g, int V) {
    vector<vector<int>> adj(V);
    for (auto [u, v] : g)
        adj[v].push_back(u);
    return adj;
}

auto make_adjacency_set_undirected(const edges_t& g) {
    unordered_set<array<int, 2>> adj;
    for (auto [u, v] : g)
        adj.insert({u, v}), adj.insert({v, u});
    return adj;
}

auto make_adjacency_set_directed(const edges_t& g) {
    unordered_set<array<int, 2>> adj;
    for (auto [u, v] : g)
        adj.insert({u, v});
    return adj;
}

auto make_adjacency_set_reverse(const edges_t& g) {
    unordered_set<array<int, 2>> adj;
    for (auto [u, v] : g)
        adj.insert({v, u});
    return adj;
}

/**
 * Check if a graph is (strongly) connected
 */
int count_reachable(const vector<vector<int>>& adj, int s = 0) {
    int i = 0, S = 1, V = adj.size();
    vector<int> bfs{s};
    vector<bool> vis(V, false);
    vis[s] = true;
    while (i++ < S && S < V) {
        for (int v : adj[bfs[i - 1]]) {
            if (!vis[v]) {
                vis[v] = true, S++;
                bfs.push_back(v);
            }
        }
    }
    return S;
}

bool reachable(const vector<vector<int>>& adj, int s, int t) {
    uint i = 0, S = 1, V = adj.size();
    vector<bool> vis(V, false);
    vector<int> bfs{s};
    vis[s] = true;
    while (i++ < S && S < V) {
        for (int v : adj[bfs[i - 1]]) {
            if (!vis[v]) {
                vis[v] = true, S++;
                bfs.push_back(v);
                if (v == t)
                    return true;
            }
        }
    }
    return false;
}

bool is_connected_undirected(const edges_t& g, int V) {
    assert(V > 0);
    auto adj = make_adjacency_lists_undirected(g, V);
    return count_reachable(adj) == V;
}

bool is_connected_directed(const edges_t& g, int V) {
    assert(V > 0);
    auto adj = make_adjacency_lists_directed(g, V);
    if (count_reachable(adj) != V)
        return false;
    adj = make_adjacency_lists_reverse(g, V);
    return count_reachable(adj) == V;
}

bool is_rooted_directed(const edges_t& g, int V, int s = 0) {
    assert(V > 0);
    auto adj = make_adjacency_lists_directed(g, V);
    return count_reachable(adj, s) == V;
}

#endif // GRAPH_OPERATIONS_HPP
