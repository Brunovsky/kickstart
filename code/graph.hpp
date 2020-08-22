#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Very simple graph data structure templates
 */

// Simple undirected graph
struct graph {
    int V, E;
    vector<vector<int>> adj;

    explicit graph(int V = 0) : V(V), E(0), adj(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(v);
        adj[v].push_back(u);
        E++;
    }
};

// Simple directed graph
struct digraph {
    int V, E;
    vector<vector<int>> adj, rev;

    explicit digraph(int V = 0) : V(V), E(0), adj(V), rev(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
        rev[v].push_back(u);
        E++;
    }
};

// Edge-list undirected graph
struct edge_graph {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source, target;

    explicit edge_graph(int V = 0) : V(V), E(0), adj(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(E);
        adj[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        E++;
    }
};

// Edge-list directed graph
struct edge_digraph {
    int V, E;
    vector<vector<int>> adj, rev;
    vector<int> source, target;

    explicit edge_digraph(int V = 0) : V(V), E(0), adj(V), rev(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        E++;
    }
};

// Weighed edge undirected graph
struct weight_graph {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source, target;
    vector<long> weight;

    explicit weight_graph(int V = 0) : V(V), E(0), adj(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long w = 0) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back(E);
        adj[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }
};

// Weighed edge directed graph
struct weight_digraph {
    int V, E;
    vector<vector<int>> adj, rev;
    vector<int> source, target;
    vector<long> weight;

    explicit weight_digraph(int V = 0) : V(V), E(0), adj(V), rev(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long w = 0) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }
};

// Directed flow graph with separate residual network
struct flow_graph {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit flow_graph(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }
};

// Directed weighted flow graph with separate residual network
struct weight_flow_graph {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap, weight;

    explicit weight_flow_graph(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1, long w = 0) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0 && w >= 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        weight.push_back(w), weight.push_back(w);
        E++;
    }
};

// Bipartite graph
struct bipartite_graph {
    int U, V, E;
    vector<vector<int>> adj, rev;

    explicit bipartite_graph(int U = 0, int V = 0) : U(U), V(V), E(0), adj(U), rev(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
        rev[v].push_back(u);
        E++;
    }
};

// Edge bipartite graph
struct bipartite_edge_graph {
    int U, V, E;
    vector<vector<int>> adj, rev;
    vector<int> source, target;

    explicit bipartite_edge_graph(int U = 0, int V = 0)
        : U(U), V(V), E(0), adj(U), rev(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        E++;
    }
};

// Weighed bipartite graph
struct weight_bipartite_graph {
    int U, V, E;
    vector<vector<int>> adj, rev;
    vector<int> source, target;
    vector<long> weight;

    explicit weight_bipartite_graph(int U = 0, int V = 0)
        : U(U), V(V), E(0), adj(U), rev(V) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }
};

#endif // GRAPH_HPP
