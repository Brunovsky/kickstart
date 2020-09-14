#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "bits.hpp"
#include "graph_convert.hpp"
#include "graph_operations.hpp"
#include "hash.hpp"
#include "math.hpp"
#include "random.hpp"
#include "regular.hpp"

// *****

// ***** Auxiliary methods

edgeset_t build_adjacency_set(const graph& g) {
    unordered_set<array<int, 2>, pair_hasher> edgeset;
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            edgeset.insert({u, v});
    return edgeset;
}

/** Basic auxiliary methods for adding edges to graphs
 */

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

void add_uniform_edges(bipartite_graph& g, int u1, int u2, int v1, int v2, double p) {
    if (u2 - u1 >= 10 && v2 - v1 >= 10 && p <= 0.25) {
        binomd distk(v2 - v1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), v1, v2 - 1))
                if (u != v)
                    g.add(u, v);
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = v1; v < v2; v++)
                if (distp(mt))
                    g.add(u, v);
    }
}

void add_any_missing_edge(graph& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        if (g.adj[u].empty())
            g.add(u, different(u, v1, v2 - 1));
}

void add_any_missing_outedge(digraph& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        if (g.adj[u].empty())
            g.add(u, different(u, v1, v2 - 1));
}

void add_any_missing_inedge(digraph& g, int u1, int u2, int v1, int v2) {
    for (int v = v1; v < v2; v++)
        if (g.rev[v].empty())
            g.add(different(v, u1, u2 - 1), v);
}

void add_any_missing_outedge(bipartite_graph& g, int u1, int u2, int v1, int v2) {
    intd distv(v1, v2 - 1);
    for (int u = u1; u < u2; u++)
        if (g.adj[u].empty())
            g.add(u, distv(mt));
}

void add_any_missing_inedge(bipartite_graph& g, int u1, int u2, int v1, int v2) {
    intd distu(u1, u2 - 1);
    for (int v = v1; v < v2; v++)
        if (g.rev[v].empty())
            g.add(distu(mt), v);
}

void add_edges(graph& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        assert(u < v);
        g.add(u, v);
    }
}

void add_edges(digraph& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        assert(u != v);
        g.add(u, v);
    }
}

void add_edges(bipartite_graph& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        g.add(u, v);
    }
}

void add_nontrivial_edges(graph& g, const edges_t& edges, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v)
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_nontrivial_edges(digraph& g, const edges_t& edges, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v)
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_edges_except(graph& g, const edges_t& edges, const parent_t& par, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && u != par[v] && v != par[u])
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_edges_except(digraph& g, const edges_t& edges, const parent_t& par, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && u != par[v] && v != par[u])
            g.add(u, v), S--;
    }
    assert(S == 0);
}

void add_all_edges(graph& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u + 1; v < u2; v++)
            g.add(u, v);
}

void add_all_edges(digraph& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u + 1; v < u2; v++)
            g.add(u, v);
}

void add_all_edges(bipartite_graph& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        for (int v = v1; v < v2; v++)
            g.add(u, v);
}

void add_all_edges_bidirectional(digraph& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u1; v < u2; v++)
            if (u != v)
                g.add(u, v);
}

/** More exotic methods for adding edges to graphs
 */

template <typename Graph>
void add_uniform_self_loops(Graph& g, double p) {
    if (g.V >= 10 && p <= 0.25) {
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

void remove_uniform(graph& g, double p) {
    graph f(g.V);
    boold distp(p);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            if (u < v && !distp(mt))
                f.add(u, v);
    g = move(f);
}

void remove_uniform(digraph& g, double p) {
    digraph f(g.V);
    boold distp(p);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            if (!distp(mt))
                f.add(u, v);
    g = move(f);
}

void remove_uniform(bipartite_graph& g, double p) {
    bipartite_graph f(g.V);
    boold distp(p);
    for (int u = 0; u < g.V; u++)
        for (int v : g.adj[u])
            if (!distp(mt))
                f.add(u, v);
    g = move(f);
}

void add_circulant_arcs(graph& g, int u1, int u2, int o = 1) {
    int V = u2 - u1, cap = 2 * o == V ? V / 2 : V;
    for (int s = 0; s < cap; s++)
        g.add(u1 + s, u1 + (s + o) % V);
}

void add_regular_ring_lattice(graph& g, int k) {
    int max_jump = min(k / 2, (g.V - 1) / 2);
    for (int o = 1; o <= max_jump; o++)
        add_circulant_arcs(g, 0, g.V, o);
}

/** Auxiliary methods for 2D and 3D grid graphs and their edges
 *
 * 24 25 26 27 28 29
 * 18 19 20 21 22 23
 * 12 13 14 15 16 17   X=6, Y=5
 *  6  7  8  9 10 11   19=(1,3)
 *  0  1  2  3  4  5
 */

inline int calc_grid2(int X, int x, int y) { return y * X + x; }
inline int calc_grid3(int X, int Y, int x, int y, int z) { return z * X * Y + y * X + x; }

template <typename Graph>
void add_grid_edges(Graph& g, int X, int Y, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && dx <= X && 0 <= dy && dy <= Y);
    for (int x = 0; x + dx < X; x++)
        for (int y = 0; y + dy < Y; y++)
            g.add(calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy));
}

template <typename Graph>
void add_circular_grid_edges(Graph& g, int X, int Y, bool dx, bool dy) {
    assert((dx || dy) && 0 <= dx && 0 <= dy);
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            g.add(calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y));
}

template <typename Graph>
void add_grid3_edges(Graph& g, int X, int Y, int Z, bool dx, bool dy, bool dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    for (int x = 0; x + dx < X; x++)
        for (int y = 0; y + dy < Y; y++)
            for (int z = 0; z + dz < Z; z++)
                g.add(calc_grid3(X, Y, x, y, z),
                      calc_grid3(X, Y, x + dx, y + dy, z + dz));
}

template <typename Graph>
void add_circular_grid3_edges(Graph& g, int X, int Y, int Z, bool dx, bool dy, bool dz) {
    assert((dx || dy || dz) && 0 <= dx && 0 <= dy && 0 <= dz);
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            for (int z = 0; z < Z; z++)
                g.add(calc_grid3(X, Y, x, y, z),
                      calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z));
}

template <typename Graph>
void add_uniform_grid_edges(Graph& g, int X, int Y, double p, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && dx <= X && 0 <= dy && dy <= Y);
    if (dx < X && dy < Y && p <= 0.25) {
        binomd distk((X - dx) * (Y - dy), p);
        auto edges = pair_sample(distk(mt), 0, X - dx - 1, 0, Y - dy - 1);
        for (auto [x, y] : edges)
            g.add(calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy));
    } else {
        boold distp(p);
        for (int x = 0; x + dx < X; x++)
            for (int y = 0; y + dy < Y; y++)
                if (distp(mt))
                    g.add(calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy));
    }
}

template <typename Graph>
void add_uniform_circular_grid_edges(Graph& g, int X, int Y, double p, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && 0 <= dy);
    if (dx < X && dy < Y && p <= 0.25) {
        binomd distk(X * Y, p);
        auto edges = pair_sample(distk(mt), 0, X - 1, 0, Y - 1);
        for (auto [x, y] : edges)
            g.add(calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y));
    } else {
        boold distp(p);
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                if (distp(mt))
                    g.add(calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y));
    }
}

template <typename Graph>
void add_uniform_grid3_edges(Graph& g, int X, int Y, int Z, double p, int dx, int dy,
                             int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    if (dx < X && dy < Y && dz < Z && p <= 0.25) {
        for (int z = 0; z + dz < Z; z++) {
            binomd distk((X - dx) * (Y - dy), p);
            auto edges = pair_sample(distk(mt), 0, X - dx - 1, 0, Y - dy - 1);
            for (auto [x, y] : edges)
                g.add(calc_grid3(X, Y, x, y, z),
                      calc_grid3(X, Y, x + dx, y + dy, z + dz));
        }
    } else {
        boold distp(p);
        for (int x = 0; x + dx < X; x++)
            for (int y = 0; y + dy < Y; y++)
                for (int z = 0; z + dz < Z; z++)
                    if (distp(mt))
                        g.add(calc_grid3(X, Y, x, y, z),
                              calc_grid3(X, Y, x + dx, y + dy, z + dz));
    }
}

template <typename Graph>
void add_uniform_circular_grid3_edges(Graph& g, int X, int Y, int Z, double p, int dx,
                                      int dy, int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    if (dx < X && dy < Y && dz < Z && p <= 0.25) {
        for (int z = 0; z < Z; z++) {
            binomd distk(X * Y, p);
            auto edges = pair_sample(distk(mt), 0, X - 1, 0, Y - 1);
            for (auto [x, y] : edges)
                g.add(calc_grid3(X, Y, x, y, z),
                      calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z));
        }
    } else {
        boold distp(p);
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                    if (distp(mt))
                        g.add(calc_grid3(X, Y, x, y, z),
                              calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z));
    }
}

/** Auxiliary methods for level (ranked) graphs and complete multipartite graphs
 */

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

/** Simple graph types like trees and grids with exact, known shape
 */

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
    add_all_edges(g, 0, U, 0, V);
    return g;
}

/** More complex graphs with known shape
 */

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

graph star(int n) {
    graph g(n);
    add_level_step_full(g, 0, 1, 1, n);
    return g;
}

graph stacked_book(int m, int n) {
    int k = (m + 1) * n;
    graph g(k);
    for (int i = 0; i < k; i += m + 1)
        add_level_step_full(g, i, i + 1, i + 1, i + m + 1);
    for (int i = m + 1; i < k; i++)
        g.add(i - (m + 1), i);
    return g;
}

graph book(int m) {
    // special case of stacked book
    return stacked_book(m, 2);
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

/** Randomized known shape graphs
 */

graph random_tree_undirected(int V) {
    graph g(V);
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1);
    return g;
}

digraph random_tree_directed(int V, bool toparent = true, bool tochild = false) {
    digraph g(V);
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, toparent, tochild);
    return g;
}

graph random_regular(int V, int k) {
    graph g(V);
    auto edges = regular_sample(V, k);
    add_edges(g, edges);
    return g;
}

graph random_regular_connected(int V, int k) {
    graph g;
    do {
        g = random_regular(V, k);
    } while (!is_connected(g));
    return g;
}

digraph random_regular_directed(int V, int k) {
    digraph g(V);
    auto edges = regular_directed_sample(V, k);
    add_edges(g, edges);
    return g;
}

digraph random_regular_directed_connected(int V, int k) {
    digraph g;
    do {
        g = random_regular_directed(V, k);
    } while (!is_connected(g));
    return g;
}

bipartite_graph random_regular_bipartite(int U, int V, int k) {
    bipartite_graph g(U, V);
    auto edges = regular_bipartite_sample(U, V, k);
    add_edges(g, edges);
    return g;
}

/** Completely random and not necessarily connected graphs (Erdős–Rényi graphs)
 */

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

bipartite_graph random_uniform_bipartite(int U, int V, double p) {
    bipartite_graph g(U, V);
    add_uniform_edges(g, 0, U, 0, V, min(1.0, p));
    return g;
}

graph random_exact_undirected(int V, int E) {
    assert(E <= 1L * V * (V - 1) / 2);
    graph g(V);
    auto edges = choose_sample(E, 0, V - 1, false);
    add_edges(g, edges);
    assert(g.E == E);
    return g;
}

digraph random_exact_directed(int V, int E) {
    assert(E <= 1L * V * (V - 1));
    digraph g(V);
    auto edges = pair_sample(E + V, 0, V - 1, 0, V - 1);
    shuffle(begin(edges), end(edges), mt);
    add_nontrivial_edges(g, edges, E);
    assert(g.E == E);
    return g;
}

bipartite_graph random_exact_bipartite(int U, int V, int E) {
    assert(E <= 1L * V * (V - 1));
    bipartite_graph g(U, V);
    auto edges = pair_sample(E, 0, U - 1, 0, V - 1);
    add_edges(g, edges);
    assert(g.E == E);
    return g;
}

/** Completely random but connected graphs (Erdős–Rényi with bootstrap tree)
 */

graph random_uniform_undirected_connected(int V, double p) {
    graph g(V);
    auto parent = parent_sample(V);
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
    auto parent = parent_sample(V);
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
    auto parent = parent_sample(V);
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

    auto parent = parent_sample(V);
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

/** Many complete graphs joined in different ways
 */

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

/** Level (ranked) connected graphs, optionally looped
 *
 * Each level is connected (full,uniform) to the next level.
 */

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

// ***** Convert to flow network (temporary)

flow_graph random_flow_graph(int V, double p, long max_cap) {
    intd rankd(3, max(3, V / 3));
    int ranks = rankd(mt), m = min(7, V / ranks);
    p = min(1.0, p);
    digraph g = random_uniform_level_flow(V, p, ranks, m, false);
    return add_caps(g, max_cap);
}

#endif // GRAPH_GENERATOR_HPP
