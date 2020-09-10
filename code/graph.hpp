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
    int V, E = 0;
    vector<vector<int>> adj;

    explicit graph(int V = 0) : V(V), adj(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        adj[u].push_back(v);
        adj[v].push_back(u);
        E++;
    }
};

// Simple directed graph
struct digraph {
    int V, E = 0;
    vector<vector<int>> adj, rev;

    explicit digraph(int V = 0) : V(V), adj(V), rev(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
        rev[v].push_back(u);
        E++;
    }
};

// Edge-list undirected graph
struct edge_graph {
    int V, E = 0;
    vector<vector<int>> adj;
    vector<int> source, target;

    explicit edge_graph(int V = 0) : V(V), adj(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

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
    int V, E = 0;
    vector<vector<int>> adj, rev;
    vector<int> source, target;

    explicit edge_digraph(int V = 0) : V(V), adj(V), rev(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

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
struct cost_graph {
    int V, E = 0;
    vector<vector<int>> adj;
    vector<int> source, target;
    vector<long> cost;

    explicit cost_graph(int V = 0) : V(V), adj(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back(E);
        adj[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        cost.push_back(w);
        E++;
    }
};

// Weighed edge directed graph
struct cost_digraph {
    int V, E = 0;
    vector<vector<int>> adj, rev;
    vector<int> source, target;
    vector<long> cost;

    explicit cost_digraph(int V = 0) : V(V), adj(V), rev(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        cost.push_back(w);
        E++;
    }
};

// Directed flow graph with separate residual network
struct flow_graph {
    int V, E = 0;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit flow_graph(int V = 0) : V(V), adj(V), rev(V), res(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        cap.push_back(c), cap.push_back(0);
        E++;
    }
};

// Directed weighted flow graph with separate residual network
struct cost_flow_graph {
    int V, E = 0;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap, cost;

    explicit cost_flow_graph(int V = 0) : V(V), adj(V), rev(V), res(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0 && w >= 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        cap.push_back(c), cap.push_back(0);
        cost.push_back(w), cost.push_back(w);
        E++;
    }
};

// Directed supply/demand graph for one commodity with separate residual network
struct supply_graph {
    int V, E = 0;
    vector<vector<int>> adj, rev, res;
    vector<int> supply, source, target;
    vector<long> flow, cap;

    explicit supply_graph(int V = 0) : V(V), adj(V), rev(V), res(V), supply(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        cap.push_back(c), cap.push_back(0);
        E++;
    }
};

// Directed weighed supply/demand graph for one commodity with separate residual network
struct cost_supply_graph {
    int V, E = 0;
    vector<vector<int>> adj, rev, res;
    vector<int> supply, source, target;
    vector<long> flow, cap, cost;

    explicit cost_supply_graph(int V = 0) : V(V), adj(V), rev(V), res(V), supply(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0 && w >= 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        cap.push_back(c), cap.push_back(0);
        cost.push_back(w), cost.push_back(-w);
        E++;
    }
};

// Bipartite graph
struct bipartite_graph {
    int U, V, E = 0;
    vector<vector<int>> adj, rev;

    explicit bipartite_graph(int U = 0, int V = 0) : U(U), V(V), adj(U), rev(V) {}

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
        rev[v].push_back(u);
        E++;
    }
};

// Edge bipartite graph
struct bipartite_edge_graph {
    int U, V, E = 0;
    vector<vector<int>> adj, rev;
    vector<int> source, target;

    explicit bipartite_edge_graph(int U = 0, int V = 0) : U(U), V(V), adj(U), rev(V) {}

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
struct cost_bipartite_graph {
    int U, V, E = 0;
    vector<vector<int>> adj, rev;
    vector<int> source, target;
    vector<long> cost;

    explicit cost_bipartite_graph(int U = 0, int V = 0) : U(U), V(V), adj(U), rev(V) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        adj[u].push_back(E);
        rev[v].push_back(E);
        source.push_back(u);
        target.push_back(v);
        cost.push_back(w);
        E++;
    }
};

#endif // GRAPH_HPP
