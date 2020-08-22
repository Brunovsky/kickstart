#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "graph_operations.hpp"

// *****

// ***** Auxiliary methods

template <typename Graph>
void add_self_loops(Graph& g, double p) {
    if (p <= 0.15) {
        binomd distk(g.V, p);
        for (int u : int_sample(distk(mt), 0, g.V - 1))
            g.add(u, u);
    } else {
        boold distp(p);
        for (int u = 0; u < g.V; u++)
            if (distp(mt))
                g.add(u, u);
    }
}

void add_parent_edges(graph& g, const vector<int>& parent, int start) {
    for (int u = start; u < g.V; u++) {
        g.add(u, parent[u]);
    }
}

void add_parent_edges(digraph& g, const vector<int>& parent, int start,
                      bool toparent = true, bool tochild = false) {
    for (int u = start; u < g.V; u++) {
        if (toparent)
            g.add(u, parent[u]);
        if (tochild)
            g.add(parent[u], u);
    }
}

void add_level_back_edges(digraph& g, double q, const vector<int>& R) {
    boold distq(q);
    int start = 0, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        int mid = start + R[r];
        int universe = g.V - mid;
        binomd distk(universe, q);
        for (int u = start; u < mid; u++) {
            int k = distk(mt);
            for (int v : int_sample(k, mid, g.V - 1)) {
                g.add(v, u);
            }
        }
        start = mid;
    }
}

template <typename Graph>
void add_level_step_full(Graph& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        for (int v = v1; v < v2; v++)
            g.add(u, v);
}

template <typename Graph>
void add_level_step_uniform(Graph& g, int u1, int u2, int v1, int v2, double p,
                            bool mustout = false, bool mustin = false) {
    if (u1 == u2 || v1 == v2)
        return;
    vector<bool> out(u2 - u1, false), in(v2 - v1, false);
    if (p <= 0.25) {
        binomd distk(v2 - v1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), v1, v2 - 1))
                g.add(u, v), out[u - u1] = true, in[v - v1] = true;
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = v1; v < v2; v++)
                if (distp(mt))
                    g.add(u, v), out[u - u1] = true, in[v - v1] = true;
    }
    if (mustout) {
        intd distv(v1, v2 - 1);
        for (int v, u = u1; u < u2; u++)
            if (!out[u - u1])
                v = distv(mt), g.add(u, v), in[v - v1] = true;
    }
    if (mustin) {
        intd distu(u1, u2 - 1);
        for (int v = v1; v < v2; v++)
            if (!in[v - v1])
                g.add(distu(mt), v);
    }
}

template <typename Graph>
void link_levels_full(Graph& g, const vector<int>& R, bool loop = false) {
    int start = 0, ranks = R.size();
    for (int r = 0; r + 1 < ranks; r++) {
        int mid = start + R[r], end = mid + R[r + 1];
        add_level_step_full(g, start, mid, mid, end);
        start = mid;
    }
    assert(start != g.V);
    if (loop && ranks >= 2) {
        int mid = start + R[ranks - 1];
        assert(mid == g.V);
        add_level_step_full(g, start, mid, 0, R[0]);
    }
}

template <typename Graph>
void link_levels_uniform(Graph& g, double p, const vector<int>& R, bool loop = false,
                         bool mustout = true, bool mustin = true) {
    int start = 0, ranks = R.size();
    for (int r = 0; r + 1 < ranks; r++) {
        int mid = start + R[r], end = mid + R[r + 1];
        add_level_step_uniform(g, start, mid, mid, end, p, mustout, mustin);
        start = mid;
    }
    assert(start != g.V);
    if (loop && ranks >= 2) {
        int mid = start + R[ranks - 1];
        assert(mid == g.V);
        add_level_step_uniform(g, start, mid, 0, R[0], p, mustout, mustin);
    }
}

template <typename Graph>
void link_levels_exp(Graph& g, double p, const vector<int>& R, bool loop = false,
                     bool mustout = true, bool mustin = true) {
    int ranks = R.size();
    vector<int> starts(ranks + 1);
    for (int r = 0; r < ranks; r++) {
        starts[r + 1] = starts[r] + R[r];
    }
    for (int r = 0; r + 1 < ranks; r++) {
        int u1 = starts[r], u2 = starts[r + 1];
        double q = 1.0;
        for (int t = r + 1; t < ranks; t++) {
            int v1 = starts[t], v2 = starts[t + 1];
            q *= p;
            bool out = mustout && t == r + 1, in = mustin && t == r + 1;
            add_level_step_uniform(g, u1, u2, v1, v2, q, out, in);
        }
    }
    if (loop && ranks >= 2) {
        double z = 1.0;
        for (int t = ranks - 1; t >= 1; t--) {
            int u1 = starts[t], u2 = starts[t + 1];
            double q = z;
            for (int r = 0; r < t; r++) {
                int v1 = starts[r], v2 = starts[r + 1];
                q *= p;
                add_level_step_uniform(g, u1, u2, v1, v2, q, false, false);
            }
            z *= p;
        }
    }
}

// ***** Simple graph types like trees and grids

graph generate_tree_undirected(int V) {
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    return g;
}

digraph generate_tree_directed(int V, bool toparent = true, bool tochild = false) {
    digraph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, toparent, tochild);
    return g;
}

graph generate_grid_undirected(int W, int H) {
    int V = W * H;
    graph g(V);
    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            int u = i * H + j;
            int south = u + H;
            int east = u + 1;
            if (i + 1 < W)
                g.add(u, south);
            if (j + 1 < H)
                g.add(u, east);
        }
    }
    return g;
}

digraph generate_grid_directed(int W, int H) {
    int V = W * H;
    digraph g(V);
    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            int u = i * H + j;
            int south = u + H;
            int east = u + 1;
            if (i + 1 < W)
                g.add(u, south);
            if (j + 1 < H)
                g.add(u, east);
        }
    }
    return g;
}

graph generate_complete_undirected(int V) {
    graph g(V);
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            g.add(u, v);
        }
    }
    return g;
}

digraph generate_complete_directed(int V) {
    digraph g(V);
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            g.add(u, v);
        }
    }
    return g;
}

graph generate_cycle_undirected(int V) {
    graph g(V);
    for (int u = 1; u < V; u++) {
        g.add(u - 1, u);
    }
    if (V >= 3)
        g.add(V - 1, 0);
    return g;
}

digraph generate_cycle_directed(int V) {
    digraph g(V);
    for (int u = 1; u < V; u++) {
        g.add(u - 1, u);
    }
    if (V >= 3)
        g.add(V - 1, 0);
    return g;
}

graph generate_regular(int V, int k) {
    graph g(V);
    auto edges = regular_sample(V, k);
    for (auto edge : edges)
        g.add(edge[0], edge[1]);
    return g;
}

// ***** Completely random but connected graphs

graph generate_uniform_undirected(int V, double p) {
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.add(u, v);
    }
    return g;
}

digraph generate_uniform_rooted_dag(int V, double p) {
    digraph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, false, true);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.add(u, v);
    }
    return g;
}

graph generate_exact_undirected(int V, int E) {
    assert(V - 1 <= E && E <= V * (V - 1) / 2);
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    if (E == V - 1)
        return g;
    int k = min(V * (V - 1) / 2, E + V);
    auto edges = choose_sample(k, 0, V - 1, false);
    shuffle(begin(edges), end(edges), mt);
    for (auto edge : edges) {
        int u = edge[0], v = edge[1];
        assert(u < v);
        if (u != parent[v] && v != parent[u])
            g.add(u, v);
        if (g.E == E)
            break;
    }
    return g;
}

digraph generate_exact_rooted_dag(int V, int E) {
    assert(V - 1 <= E && E <= V * (V - 1) / 2);
    digraph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, false, true);
    if (E == V - 1)
        return g;
    int k = min(V * (V - 1) / 2, E + V);
    auto edges = choose_sample(k, 0, V - 1, false);
    shuffle(begin(edges), end(edges), mt);
    for (auto edge : edges) {
        int u = edge[0], v = edge[1];
        assert(u < v);
        if (u != parent[v])
            g.add(u, v);
        if (g.E == E)
            break;
    }
    return g;
}

// ***** Level (ranked) connected graphs, possibly looped

graph generate_full_level(int V, int ranks, int m = 1, bool loop = false) {
    graph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    return g;
}

digraph generate_full_level_dag(int V, int ranks, int m = 1, bool loop = false) {
    digraph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    return g;
}

digraph generate_full_level_flow(int V, int ranks, int m = 1, bool loop = false) {
    digraph g(V);
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_full(g, R, loop);
    return g;
}

graph generate_uniform_level(int V, double p, int ranks, int m = 1, bool loop = false) {
    graph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop);
    return g;
}

digraph generate_uniform_level_dag(int V, double p, int ranks, int m = 1,
                                   bool loop = false) {
    digraph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    return g;
}

digraph generate_uniform_level_flow(int V, double p, int ranks, int m = 1,
                                    bool loop = false) {
    digraph g(V);
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    return g;
}

digraph generate_exp_level_flow(int V, double p, int ranks, int m = 1,
                                bool loop = false) {
    digraph g(V);
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_exp(g, p, R, loop, true, true);
    return g;
}

// ***** Strongly-connected components expansion

template <typename Gn, typename En>
digraph generate_scc_expansion(const digraph& dag, Gn&& f, En&& h) {
    int V = dag.V;
    vector<int> intv(V + 1, 0);
    digraph g(0);

    for (int u = 0; u < V; u++) {
        join(g, f(u));
        intv[u + 1] = g.V;
    }
    for (int u = 0; u < V; u++) {
        for (int v : dag.adj[u]) {
            vector<array<int, 2>> choices;
            for (int su = intv[u]; su < intv[u + 1]; su++) {
                for (int sv = intv[v]; sv < intv[v + 1]; sv++) {
                    choices.push_back({su, sv});
                }
            }
            shuffle(begin(choices), end(choices), mt);
            int edges = h(u, v);
            for (int e = 0; e < edges; e++) {
                g.add(choices[e][0], choices[e][1]);
            }
        }
    }
    return g;
}

digraph generate_scc_uniform_expansion(const digraph& dag, int k, double p) {
    int V = dag.V;
    vector<int> cnt(V);
    intd dist(1, k);
    for (int u = 0; u < V; u++) {
        cnt[u] = dist(mt);
    }
    auto f = [&](int u) {
        digraph g;
        intd ranksd(1, max(cnt[u] / 2, 1));
        g = generate_uniform_level_dag(cnt[u], p, ranksd(mt), 1, true);
        return g;
    };
    auto h = [&](int u, int v) {
        return int(ceil(sqrt(cnt[u] * cnt[v])));
    };
    return generate_scc_expansion(dag, f, h);
}

// ***** Convert to flow network

flow_graph make_flow_graph(const digraph& g, long max_cap) {
    longd capd(1, max_cap);
    flow_graph f(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            f.add(u, v, capd(mt));
        }
    }
    return f;
}

flow_graph generate_flow_graph(int V, double p, long max_cap) {
    intd rankd(3, max(3, V / 3));
    int ranks = rankd(mt), m = min(7, V / ranks);
    p = min(1.0, p);
    digraph g = generate_exp_level_flow(V, p, ranks, m, false);
    return make_flow_graph(g, max_cap);
}

#endif // GRAPH_GENERATOR_HPP
