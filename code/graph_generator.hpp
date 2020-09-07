#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "bits.hpp"
#include "graph_operations.hpp"
#include "hash.hpp"
#include "math.hpp"
#include "random.hpp"

// *****

using edges_t = vector<array<int, 2>>;
using parent_t = vector<int>;
using ranks_t = vector<int>;
using offsets_t = vector<int>;

// ***** Auxiliary methods

unordered_set<array<int, 2>, pair_hasher> build_adjacency_set(const graph& g) {
    unordered_set<array<int, 2>, pair_hasher> edgeset;
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            edgeset.insert({u, v});
    return edgeset;
}

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

void add_parent_edges(graph& g, const parent_t& parent, int start) {
    for (int u = start; u < g.V; u++) {
        g.add(u, parent[u]);
    }
}

void add_parent_edges(digraph& g, const parent_t& parent, int start, bool toparent = true,
                      bool tochild = false) {
    for (int u = start; u < g.V; u++) {
        if (toparent)
            g.add(u, parent[u]);
        if (tochild)
            g.add(parent[u], u);
    }
}

void add_uniform_edges(graph& g, int u1, int u2, double p) {
    if (u2 - u1 >= 10 && p <= 0.25) {
        for (int u = u1 + 1; u < u2; u++) {
            binomd distk(u - u1, p);
            for (int v : int_sample(distk(mt), u1, u - 1))
                g.add(u, v);
        }
    } else {
        boold distp(p);
        for (int u = u1 + 1; u < u2; u++)
            for (int v = u1; v < u; v++)
                if (distp(mt))
                    g.add(u, v);
    }
}

void add_uniform_edges(digraph& g, int u1, int u2, double p) {
    if (u2 - u1 >= 10 && p <= 0.25) {
        binomd distk(u2 - u1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), u1, u2 - 1))
                if (u != v)
                    g.add(u, v);
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = u1; v < u2; v++)
                if (u != v && distp(mt))
                    g.add(u, v);
    }
}

void add_level_back_edges(digraph& g, double q, const ranks_t& R) {
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

void add_oriented_edges(graph& g, const edges_t& edges) {
    for (auto edge : edges) {
        int u = edge[0], v = edge[1];
        assert(u < v);
        g.add(u, v);
    }
}

void add_nontrivial_edges(graph& g, const edges_t& edges, int S) {
    for (auto edge : edges) {
        if (S == 0)
            break;
        int u = edge[0], v = edge[1];
        if (u != v)
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_nontrivial_edges(digraph& g, const edges_t& edges, int S) {
    for (auto edge : edges) {
        if (S == 0)
            break;
        int u = edge[0], v = edge[1];
        if (u != v)
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_edges_except(graph& g, const edges_t& edges, const parent_t& par, int S) {
    for (auto edge : edges) {
        if (S == 0)
            break;
        int u = edge[0], v = edge[1];
        if (u != v && u != par[v] && v != par[u])
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_edges_except(digraph& g, const edges_t& edges, const parent_t& par, int S) {
    for (auto edge : edges) {
        if (S == 0)
            break;
        int u = edge[0], v = edge[1];
        if (u != v && u != par[v] && v != par[u])
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_regular_ring_lattice(graph& g, int k) {
    int max_jump = min(k / 2, (g.V - 1) / 2);
    for (int j = 1; j <= max_jump; j++) {
        for (int u = 0; u < g.V; u++) {
            int v = (u + j) - (u + j >= g.V ? g.V : 0);
            g.add(u, v);
        }
    }
}

void add_circulant_arcs(graph& g, int u1, int u2, int o = 1) {
    int V = u2 - u1, cap = 2 * o == V ? V / 2 : V;
    for (int s = 0; s < cap; s++)
        g.add(u1 + s, u1 + (s + o) % V);
}

template <typename Graph>
void add_all_edges(Graph& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u + 1; v < u2; v++)
            g.add(u, v);
}

void add_all_edges_bidirectional(digraph& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u1; v < u2; v++)
            if (u != v)
                g.add(u, v);
}

template <typename Graph>
void add_grid_edges(Graph& g, int W, int H, bool v, bool h) {
    assert(v || h);
    for (int i = 0; i + v < W; i++)
        for (int j = 0; j + h < H; j++)
            g.add(i * H + j, (i + v) * H + (j + h));
}

template <typename Graph>
void add_circular_grid_edges(Graph& g, int W, int H, bool v, bool h) {
    assert(v || h);
    for (int i = 0; i < W; i++)
        for (int j = 0; j < H; j++)
            g.add(i * H + j, (i + v) * H % (W * H) + (j + h) % H);
}

template <typename Graph>
void add_grid3_edges(Graph& g, int X, int Y, int Z, bool x, bool y, bool z) {
    assert(x || y || z);
    for (int i = 0; i + x < X; i++)
        for (int j = 0; j + y < Y; j++)
            for (int k = 0; k + z < Z; k++)
                g.add(i * Y * Z + j * Z + k, (i + x) * Y * Z + (j + y) * Z + (k + z));
}

template <typename Graph>
void add_circular_grid3_edges(Graph& g, int X, int Y, int Z, bool x, bool y, bool z) {
    assert(x || y || z);
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++)
            for (int k = 0; k < Z; k++)
                g.add(i * Y * Z + j * Z + k, (i + x) * Y * Z % (X * Y * Z) +
                                                 (j + y) * Z % (Y * Z) + (k + z) % Z);
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
    if (long(u2 - u1) * long(v2 - v1) >= 100 && p <= 0.25) {
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
void link_levels_full(Graph& g, const ranks_t& R, bool loop = false) {
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
void link_levels_uniform(Graph& g, double p, const ranks_t& R, bool loop = false,
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

void complete_levels(graph& g, const ranks_t& R) {
    int start = 0, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        add_all_edges(g, start, start + R[r]);
        start += R[r];
    }
    assert(start == g.V);
}

void complete_levels(digraph& g, const ranks_t& R) {
    int start = 0, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        add_all_edges_bidirectional(g, start, start + R[r]);
        start += R[r];
    }
    assert(start == g.V);
}

// ***** Simple graph types like trees and grids with exact, known shape

graph path_undirected(int V) {
    graph g(V);
    for (int u = 0; u + 1 < V; u++)
        g.add(u, u + 1);
    return g;
}

digraph path_directed(int V) {
    digraph g(V);
    for (int u = 0; u + 1 < V; u++)
        g.add(u, u + 1);
    return g;
}

graph grid_undirected(int W, int H) {
    graph g(W * H);
    add_grid_edges(g, W, H, 0, 1);
    add_grid_edges(g, W, H, 1, 0);
    return g;
}

digraph grid_directed(int W, int H) {
    digraph g(W * H);
    add_grid_edges(g, W, H, 0, 1);
    add_grid_edges(g, W, H, 1, 0);
    return g;
}

graph circular_grid_undirected(int W, int H) {
    graph g(W * H);
    add_circular_grid_edges(g, W, H, 0, 1);
    add_circular_grid_edges(g, W, H, 1, 0);
    return g;
}

digraph circular_grid_directed(int W, int H) {
    digraph g(W * H);
    add_circular_grid_edges(g, W, H, 0, 1);
    add_circular_grid_edges(g, W, H, 1, 0);
    return g;
}

graph grid3_undirected(int X, int Y, int Z) {
    graph g(X * Y * Z);
    add_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

digraph grid3_directed(int X, int Y, int Z) {
    digraph g(X * Y * Z);
    add_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

graph circular_grid3_undirected(int X, int Y, int Z) {
    graph g(X * Y * Z);
    add_circular_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_circular_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_circular_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

digraph circular_grid3_directed(int X, int Y, int Z) {
    digraph g(X * Y * Z);
    add_circular_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_circular_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_circular_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

graph complete_undirected(int V) {
    graph g(V);
    add_all_edges(g, 0, V);
    return g;
}

digraph complete_directed(int V) {
    digraph g(V);
    add_all_edges(g, 0, V);
    return g;
}

graph cycle_undirected(int V) {
    graph g(V);
    for (int u = 1; u < V; u++) {
        g.add(u - 1, u);
    }
    if (V >= 3)
        g.add(V - 1, 0);
    return g;
}

digraph cycle_directed(int V) {
    digraph g(V);
    for (int u = 1; u < V; u++) {
        g.add(u - 1, u);
    }
    if (V >= 3)
        g.add(V - 1, 0);
    return g;
}

graph regular_ring(int V, int k) {
    assert(k >= 2 && k % 2 == 0 && k < V);
    graph g(V);
    add_regular_ring_lattice(g, k);
    return g;
}

graph ary_tree_undirected(int r, int V) {
    graph g(V);
    for (int i = 1; i < V; i++)
        g.add(i, (i - 1) / r);
    return g;
}

digraph ary_tree_directed(int r, int V, bool toparent = true, bool tochild = false) {
    digraph g(V);
    if (toparent)
        for (int i = 1; i < V; i++)
            g.add(i, (i - 1) / r);
    if (tochild)
        for (int i = 1; i < V; i++)
            g.add((i - 1) / r, i);
    return g;
}

graph perfect_binary_tree_undirected(int h) {
    int V = (1 << h) - 1;
    return ary_tree_undirected(2, V);
}

digraph perfect_binary_tree_directed(int h, bool toparent = true, bool tochild = false) {
    int V = (1 << h) - 1;
    return ary_tree_directed(2, V, toparent, tochild);
}

graph perfect_tree_undirected(int r, int h) {
    int V = r == 1 ? h + 1 : (intpow(r, h + 1) - 1) / (r - 1);
    return ary_tree_undirected(r, V);
}

digraph perfect_tree_directed(int r, int h, bool toparent = true, bool tochild = false) {
    int V = r == 1 ? h + 1 : (intpow(r, h + 1) - 1) / (r - 1);
    return ary_tree_directed(r, V, toparent, tochild);
}

graph complete_multipartite(const ranks_t& R) {
    int V = accumulate(begin(R), end(R), 0);
    int start = 0, ranks = R.size();
    graph g(V);
    for (int r = 0; r < ranks; r++) {
        int mid = start + R[r];
        add_level_step_full(g, start, mid, mid, V);
        start = mid;
    }
    return g;
}

bipartite_graph complete_bipartite(int U, int V) {
    bipartite_graph g(U, V);
    for (int u = 0; u < U; u++)
        for (int v = 0; v < V; v++)
            g.add(u, v);
    return g;
}

// ***** More complex graphs with known shape

graph johnson(int n, int k) {
    assert(0 < n && n <= 31 && 0 < k && k <= n);
    int V = choose(n, k);
    graph g(V);

    unordered_map<int, int> label;
    int l = 0, fmask = (1 << n) - 1;
    FOR_EACH_MASK (u, k, n) { label[u] = l++; }
    assert(l == V);

    FOR_EACH_MASK (u, k, n)
        FOR_EACH_BIT (u, a, i)
            FOR_EACH_BIT ((~u & fmask), b, j)
                if (a < b)
                    g.add(label[u], label[(u ^ i) | j]);
    assert(g.E == k * (n - k) * V / 2);
    return g;
}

graph kneser(int n, int k) {
    assert(0 < n && n <= 31 && 0 < k && k <= n);
    int V = choose(n, k);
    graph g(V);

    unordered_map<int, int> label;
    int l = 0;
    FOR_EACH_MASK (u, k, n) { label[u] = l++; }
    assert(l == V);

    // TODO: improve complexity
    FOR_EACH_MASK (u, k, n)
        FOR_EACH_MASK (v, k, n)
            if (!(u & v) && u < v)
                g.add(label[u], label[v]);
    assert(2 * g.E == V * choose(n - k, k));
    return g;
}

graph peterson() {
    // special case of kneser graph
    return kneser(5, 2);
}

graph wheel(int n) {
    assert(n >= 3);
    graph g(n + 1);
    for (int u = 0; u < n; u++) {
        g.add(u, n);
        g.add(u, u + 1 == n ? 0 : u + 1);
    }
    return g;
}

graph cogwheel(int n) {
    assert(n >= 3);
    graph g(2 * n + 1);
    for (int u = 0; u < n; u++) {
        g.add(2 * u, 2 * n);
        g.add(2 * u, 2 * u + 1);
        g.add(2 * u, u == 0 ? 2 * n - 1 : 2 * u - 1);
    }
    return g;
}

graph web(int n, int h) {
    graph g(n * h);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < n; j++) {
            int u = i * n + j;
            g.add(u, j + 1 == n ? i * n : u + 1);
            if (i + 1 < h)
                g.add(u, u + n);
        }
    }
    return g;
}

graph turan(int n, int r) {
    ranks_t R(n);
    for (int i = 0; i < r; i++)
        R[i] = (n + i) / r;
    return complete_multipartite(R);
}

graph circulant(int V, const offsets_t& O) {
    graph g(V);
    for (int o : O)
        add_circulant_arcs(g, 0, V, o);
    return g;
}

graph sudoku(int n) {
    int m = int(sqrt(n));
    assert(m * m == n);
    graph g(n * n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int u = i * n + j;
            // same col, different row
            for (int k = i + 1; k < n; k++)
                g.add(u, k * n + j);
            // same row, different col
            for (int k = j + 1; k < n; k++)
                g.add(u, i * n + k);
            // same cell, different row, different col
            int x = i / m, y = j / m;
            for (int a = x * m; a < (x + 1) * m; a++) {
                for (int b = y * m; b < (y + 1) * m; b++) {
                    int v = a * n + b;
                    if (a != i && b != j && u < v)
                        g.add(u, v);
                }
            }
        }
    }
    assert(2 * g.E == 1L * n * n * (3 * n - 1 - 2 * m));
    return g;
}

// ***** Randomized known shape graphs

graph random_tree_undirected(int V) {
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    return g;
}

digraph random_tree_directed(int V, bool toparent = true, bool tochild = false) {
    digraph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, toparent, tochild);
    return g;
}

graph random_regular(int V, int k) {
    graph g(V);
    auto edges = regular_sample(V, k);
    add_oriented_edges(g, edges);
    return g;
}

graph random_regular_connected(int V, int k) {
    graph g;
    do {
        g = random_regular(V, k);
    } while (!is_connected(g));
    return g;
}

// ***** Completely random and not necessarily connected graphs (Erdős–Rényi
// graphs)

graph random_uniform_undirected(int V, double p) {
    graph g(V);
    add_uniform_edges(g, 0, V, min(1.0, p));
    return g;
}

digraph random_uniform_directed(int V, double p) {
    digraph g(V);
    add_uniform_edges(g, 0, V, min(1.0, p));
    return g;
}

graph random_exact_undirected(int V, int E) {
    assert(E <= 1L * V * (V - 1) / 2);
    graph g(V);
    auto edges = choose_sample(E, 0, V - 1, false);
    add_oriented_edges(g, edges);
    assert(g.E == E);
    return g;
}

digraph random_exact_directed(int V, int E) {
    assert(E <= 1L * V * (V - 1));
    digraph g(V);
    auto edges = pair_sample(E + V, 0, V - 1, 0, V - 1, false);
    shuffle(begin(edges), end(edges), mt);
    add_nontrivial_edges(g, edges, E);
    assert(g.E == E);
    return g;
}

// ***** Completely random but connected graphs (Erdős–Rényi with bootstrap
// tree)

graph random_uniform_undirected_connected(int V, double p) {
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    p = min(p, 1.0);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.add(u, v);
    }
    return g;
}

digraph random_uniform_rooted_dag_connected(int V, double p) {
    digraph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, false, true);
    p = min(p, 1.0);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.add(u, v);
    }
    return g;
}

graph random_exact_undirected_connected(int V, int E) {
    assert(V - 1 <= E && E <= 1L * V * (V - 1) / 2);
    graph g(V);
    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    if (E == V - 1)
        return g;

    int k = min(1L * V * (V - 1) / 2, 0L + E + V);
    auto edges = choose_sample(k, 0, V - 1, false);
    shuffle(begin(edges), end(edges), mt);
    add_edges_except(g, edges, parent, E - g.E);
    assert(g.E == E);
    return g;
}

digraph random_exact_rooted_dag_connected(int V, int E) {
    assert(V - 1 <= E && E <= 1L * V * (V - 1) / 2);
    digraph g(V);

    vector<int> parent = parent_sample(V);
    add_parent_edges(g, parent, 1, false, true);
    if (E == V - 1)
        return g;

    int k = min(1L * V * (V - 1) / 2, 0L + E + V);
    auto edges = choose_sample(k, 0, V - 1, false);
    shuffle(begin(edges), end(edges), mt);
    add_edges_except(g, edges, parent, E - g.E);
    assert(g.E == E);
    return g;
}

// ***** Many complete graphs joined in different ways

graph disjoint_complete_undirected(int n, int k) {
    graph g(n * k);
    for (int i = 0; i < n; i++)
        add_all_edges(g, i * k, (i + 1) * k);
    return g;
}

digraph disjoint_complete_directed(int n, int k) {
    digraph g(n * k);
    for (int i = 0; i < n; i++)
        add_all_edges(g, i * k, (i + 1) * k);
    return g;
}

graph one_connected_complete_undirected(int n, int k) {
    graph g = disjoint_complete_undirected(n, k);
    for (int i = 0; i + 1 < n; i++) {
        int u = k * i + (i % k), v = k * (i + 1) + (i % k);
        g.add(u, v);
    }
    return g;
}

digraph one_connected_complete_directed(int n, int k) {
    digraph g = disjoint_complete_directed(n, k);
    for (int i = 0; i + 1 < n; i++) {
        int u = k * i + (i % k), v = k * (i + 1) + (i % k);
        g.add(u, v);
    }
    return g;
}

graph k_connected_complete_undirected(int n, int k) {
    graph g = disjoint_complete_undirected(n, k);
    for (int i = 0; i + 1 < n; i++)
        for (int j = 0; j < k; j++)
            g.add(k * i + j, k * (i + 1) + j);
    return g;
}

digraph k_connected_complete_directed(int n, int k) {
    digraph g = disjoint_complete_directed(n, k);
    for (int i = 0; i + 1 < n; i++)
        for (int j = 0; j < k; j++)
            g.add(k * i + j, k * (i + 1) + j);
    return g;
}

// ***** Level (ranked) connected graphs, optionally looped
// Each level is connected (full,uniform) to the next level.

graph random_full_level(int V, int ranks, int m = 1, bool loop = false) {
    graph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels(g, R);
    return g;
}

digraph random_full_level_dag(int V, int ranks, int m = 1, bool loop = false) {
    digraph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels(g, R);
    return g;
}

digraph random_full_level_flow(int V, int ranks, int m = 1, bool loop = false) {
    digraph g(V);
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels(g, R);
    return g;
}

graph random_uniform_level(int V, double p, int ranks, int m = 1, bool loop = false) {
    graph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop);
    complete_levels(g, R);
    return g;
}

digraph random_uniform_level_dag(int V, double p, int ranks, int m = 1,
                                 bool loop = false) {
    digraph g(V);
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    complete_levels(g, R);
    return g;
}

digraph random_uniform_level_flow(int V, double p, int ranks, int m = 1,
                                  bool loop = false) {
    digraph g(V);
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    complete_levels(g, R);
    return g;
}

// ***** Convert to flow network

flow_graph make_flow_graph(const digraph& g, long max_cap) {
    longd capd(1, max_cap);
    flow_graph f(g.V);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            f.add(u, v, capd(mt));
    return f;
}

flow_graph random_flow_graph(int V, double p, long max_cap) {
    intd rankd(3, max(3, V / 3));
    int ranks = rankd(mt), m = min(7, V / ranks);
    p = min(1.0, p);
    digraph g = random_uniform_level_flow(V, p, ranks, m, false);
    return make_flow_graph(g, max_cap);
}

#endif // GRAPH_GENERATOR_HPP
