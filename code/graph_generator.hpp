#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include "bits.hpp"
#include "graph_operations.hpp"
#include "hash.hpp"
#include "math.hpp"
#include "random.hpp"
#include "regular.hpp"

// *****

// ***** Auxiliary methods

bool verify_edges_undirected(const edges_t& g, int V, int E = -1) {
    if (E != -1 && int(g.size() != E))
        return false;
    edgeset_t edgeset;
    for (auto [u, v] : g) {
        if (edgeset.count({u, v}) || edgeset.count({v, u}))
            return false;
        if (u < 0 || V <= u || v < 0 || V <= v)
            return false;
    }
    return true;
}
bool verify_edges_directed(const edges_t& g, int V, int E = -1) {
    if (E != -1 && int(g.size() != E))
        return false;
    edgeset_t edgeset;
    for (auto [u, v] : g) {
        if (edgeset.count({u, v}))
            return false;
        if (u < 0 || V <= u || v < 0 || V <= v)
            return false;
    }
    return true;
}
bool verify_edges_bipartite(const edges_t& g, int U, int V, int E = -1) {
    if (E != -1 && int(g.size() != E))
        return false;
    edgeset_t edgeset;
    for (auto [u, v] : g) {
        if (edgeset.count({u, v}))
            return false;
        if (u < 0 || U <= u || v < 0 || V <= v)
            return false;
    }
    return true;
}

/** Basic auxiliary methods for adding edges to graphs
 */

void add_parent_edges(edges_t& g, const parent_t& parent, int u1, int u2,
                      bool toparent = true, bool tochild = false) {
    for (int u = u1; u < u2; u++) {
        if (toparent)
            g.push_back({u, parent[u]});
        if (tochild)
            g.push_back({parent[u], u});
    }
}

void add_uniform_edges_undirected(edges_t& g, int u1, int u2, double p) {
    if (u2 - u1 >= 10 && p <= 0.25) {
        for (int u = u1 + 1; u < u2; u++) {
            binomd distk(u - u1, p);
            for (int v : int_sample(distk(mt), u1, u - 1))
                g.push_back({u, v});
        }
    } else {
        boold distp(p);
        for (int u = u1 + 1; u < u2; u++)
            for (int v = u1; v < u; v++)
                if (distp(mt))
                    g.push_back({u, v});
    }
}

void add_uniform_edges_directed(edges_t& g, int u1, int u2, double p) {
    if (u2 - u1 >= 10 && p <= 0.25) {
        binomd distk(u2 - u1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), u1, u2 - 1))
                if (u != v)
                    g.push_back({u, v});
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = u1; v < u2; v++)
                if (u != v && distp(mt))
                    g.push_back({u, v});
    }
}

void add_uniform_edges_bipartite(edges_t& g, int u1, int u2, int v1, int v2, double p) {
    if (u2 - u1 >= 10 && v2 - v1 >= 10 && p <= 0.25) {
        binomd distk(v2 - v1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), v1, v2 - 1))
                if (u != v)
                    g.push_back({u, v});
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = v1; v < v2; v++)
                if (distp(mt))
                    g.push_back({u, v});
    }
}

void add_edges_undirected(edges_t& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        assert(u < v);
        g.push_back({u, v});
    }
}

void add_edges_directed(edges_t& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        assert(u != v);
        g.push_back({u, v});
    }
}

void add_edges_bipartite(edges_t& g, const edges_t& edges) {
    for (auto [u, v] : edges) {
        g.push_back({u, v});
    }
}

void add_nontrivial_edges_undirected(edges_t& g, const edges_t& edges, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v)
            g.push_back({u, v}), S--;
    }
    assert(S == 0);
}

void add_nontrivial_edges_directed(edges_t& g, const edges_t& edges, int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v)
            g.push_back({u, v}), S--;
    }
    assert(S == 0);
}

void add_edges_except_undirected(edges_t& g, const edges_t& edges, const parent_t& par,
                                 int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && u != par[v] && v != par[u])
            g.push_back({u, v}), S--;
    }
    assert(S == 0);
}

void add_edges_except_directed(edges_t& g, const edges_t& edges, const parent_t& par,
                               int S) {
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && u != par[v] && v != par[u])
            g.push_back({u, v}), S--;
    }
    assert(S == 0);
}

void add_all_edges_undirected(edges_t& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u + 1; v < u2; v++)
            g.push_back({u, v});
}

void add_all_edges_directed(edges_t& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u + 1; v < u2; v++)
            g.push_back({u, v});
}

void add_all_edges_bipartite(edges_t& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        for (int v = v1; v < v2; v++)
            g.push_back({u, v});
}

void add_all_edges_bidirectional(edges_t& g, int u1, int u2) {
    for (int u = u1; u < u2; u++)
        for (int v = u1; v < u2; v++)
            if (u != v)
                g.push_back({u, v});
}

/** More exotic methods for adding edges to graphs
 */

void add_uniform_self_loops(edges_t& g, int u1, int u2, double p) {
    if (u2 - u1 >= 20 && p <= 0.25) {
        binomd distk(u2 - u1, p);
        for (int u : int_sample(distk(mt), u1, u2 - 1))
            g.push_back({u, u});
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            if (distp(mt))
                g.push_back({u, u});
    }
}

void remove_uniform(edges_t& g, double p) {
    edges_t f;
    boold distp(p);
    for (auto [u, v] : g)
        if (!distp(mt))
            f.push_back({u, v});
    g = move(f);
}

void add_circulant_arcs(edges_t& g, int u1, int u2, int o = 1) {
    int V = u2 - u1, cap = 2 * o == V ? V / 2 : V;
    for (int s = 0; s < cap; s++)
        g.push_back({u1 + s, u1 + (s + o) % V});
}

void add_regular_ring_lattice(edges_t& g, int u1, int u2, int k) {
    int V = u2 - u1, max_jump = min(k / 2, (V - 1) / 2);
    for (int o = 1; o <= max_jump; o++)
        add_circulant_arcs(g, u1, u2, o);
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

void add_grid_edges(edges_t& g, int X, int Y, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && dx <= X && 0 <= dy && dy <= Y);
    for (int x = 0; x + dx < X; x++)
        for (int y = 0; y + dy < Y; y++)
            g.push_back({calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy)});
}

void add_circular_grid_edges(edges_t& g, int X, int Y, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && 0 <= dy);
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            g.push_back({calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y)});
}

void add_grid3_edges(edges_t& g, int X, int Y, int Z, int dx, int dy, int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    for (int x = 0; x + dx < X; x++)
        for (int y = 0; y + dy < Y; y++)
            for (int z = 0; z + dz < Z; z++)
                g.push_back({calc_grid3(X, Y, x, y, z),
                             calc_grid3(X, Y, x + dx, y + dy, z + dz)});
}

void add_circular_grid3_edges(edges_t& g, int X, int Y, int Z, int dx, int dy, int dz) {
    assert((dx || dy || dz) && 0 <= dx && 0 <= dy && 0 <= dz);
    for (int x = 0; x < X; x++)
        for (int y = 0; y < Y; y++)
            for (int z = 0; z < Z; z++)
                g.push_back({calc_grid3(X, Y, x, y, z),
                             calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z)});
}

void add_uniform_grid_edges(edges_t& g, int X, int Y, double p, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && dx <= X && 0 <= dy && dy <= Y);
    if (dx < X && dy < Y && p <= 0.25) {
        binomd distk((X - dx) * (Y - dy), p);
        auto edges = pair_sample(distk(mt), 0, X - dx - 1, 0, Y - dy - 1);
        for (auto [x, y] : edges)
            g.push_back({calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy)});
    } else {
        boold distp(p);
        for (int x = 0; x + dx < X; x++)
            for (int y = 0; y + dy < Y; y++)
                if (distp(mt))
                    g.push_back({calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy)});
    }
}

void add_uniform_circular_grid_edges(edges_t& g, int X, int Y, double p, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && 0 <= dy);
    if (dx < X && dy < Y && p <= 0.25) {
        binomd distk(X * Y, p);
        auto edges = pair_sample(distk(mt), 0, X - 1, 0, Y - 1);
        for (auto [x, y] : edges)
            g.push_back({calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y)});
    } else {
        boold distp(p);
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                if (distp(mt))
                    g.push_back(
                        {calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y)});
    }
}

void add_uniform_grid3_edges(edges_t& g, int X, int Y, int Z, double p, int dx, int dy,
                             int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    if (dx < X && dy < Y && dz < Z && p <= 0.25) {
        for (int z = 0; z + dz < Z; z++) {
            binomd distk((X - dx) * (Y - dy), p);
            auto edges = pair_sample(distk(mt), 0, X - dx - 1, 0, Y - dy - 1);
            for (auto [x, y] : edges)
                g.push_back({calc_grid3(X, Y, x, y, z),
                             calc_grid3(X, Y, x + dx, y + dy, z + dz)});
        }
    } else {
        boold distp(p);
        for (int x = 0; x + dx < X; x++)
            for (int y = 0; y + dy < Y; y++)
                for (int z = 0; z + dz < Z; z++)
                    if (distp(mt))
                        g.push_back({calc_grid3(X, Y, x, y, z),
                                     calc_grid3(X, Y, x + dx, y + dy, z + dz)});
    }
}

void add_uniform_circular_grid3_edges(edges_t& g, int X, int Y, int Z, double p, int dx,
                                      int dy, int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx <= X && 0 <= dy && dy <= Y && 0 <= dz && dz <= Z);
    if (dx < X && dy < Y && dz < Z && p <= 0.25) {
        for (int z = 0; z < Z; z++) {
            binomd distk(X * Y, p);
            auto edges = pair_sample(distk(mt), 0, X - 1, 0, Y - 1);
            for (auto [x, y] : edges)
                g.push_back({calc_grid3(X, Y, x, y, z),
                             calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z)});
        }
    } else {
        boold distp(p);
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                    if (distp(mt))
                        g.push_back(
                            {calc_grid3(X, Y, x, y, z),
                             calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z)});
    }
}

/** Auxiliary methods for level (ranked) graphs and complete multipartite graphs
 */

void add_level_step_full(edges_t& g, int u1, int u2, int v1, int v2) {
    for (int u = u1; u < u2; u++)
        for (int v = v1; v < v2; v++)
            g.push_back({u, v});
}

void add_level_step_uniform(edges_t& g, int u1, int u2, int v1, int v2, double p,
                            bool mustout = false, bool mustin = false) {
    if (u1 == u2 || v1 == v2)
        return;

    vector<bool> out(u2 - u1, false), in(v2 - v1, false);
    if (long(u2 - u1) * long(v2 - v1) >= 100 && p <= 0.25) {
        binomd distk(v2 - v1, p);
        for (int u = u1; u < u2; u++)
            for (int v : int_sample(distk(mt), v1, v2 - 1))
                g.push_back({u, v}), out[u - u1] = true, in[v - v1] = true;
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            for (int v = v1; v < v2; v++)
                if (distp(mt))
                    g.push_back({u, v}), out[u - u1] = true, in[v - v1] = true;
    }

    if (mustout) {
        intd distv(v1, v2 - 1);
        for (int v, u = u1; u < u2; u++)
            if (!out[u - u1])
                v = distv(mt), g.push_back({u, v}), in[v - v1] = true;
    }
    if (mustin) {
        intd distu(u1, u2 - 1);
        for (int v = v1; v < v2; v++)
            if (!in[v - v1])
                g.push_back({distu(mt), v});
    }
}

void link_levels_full(edges_t& g, const ranks_t& R, bool loop = false) {
    int start = 0, ranks = R.size();
    for (int r = 0; r + 1 < ranks; r++) {
        int mid = start + R[r], end = mid + R[r + 1];
        add_level_step_full(g, start, mid, mid, end);
        start = mid;
    }
    if (loop && ranks >= 2) {
        int mid = start + R[ranks - 1];
        add_level_step_full(g, start, mid, 0, R[0]);
    }
}

void link_levels_uniform(edges_t& g, double p, const ranks_t& R, bool loop = false,
                         bool mustout = true, bool mustin = true) {
    int start = 0, ranks = R.size();
    for (int r = 0; r + 1 < ranks; r++) {
        int mid = start + R[r], end = mid + R[r + 1];
        add_level_step_uniform(g, start, mid, mid, end, p, mustout, mustin);
        start = mid;
    }
    if (loop && ranks >= 2) {
        int mid = start + R[ranks - 1];
        add_level_step_uniform(g, start, mid, 0, R[0], p, mustout, mustin);
    }
}

void add_level_back_edges(edges_t& g, int u1, int u2, double q, const ranks_t& R) {
    boold distq(q);
    int start = u1, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        int mid = start + R[r];
        int universe = u2 - mid;
        binomd distk(universe, q);
        for (int u = start; u < mid; u++) {
            int k = distk(mt);
            for (int v : int_sample(k, mid, u2 - 1)) {
                g.push_back({v, u});
            }
        }
        start = mid;
    }
}

void complete_levels_undirected(edges_t& g, const ranks_t& R) {
    int start = 0, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        add_all_edges_undirected(g, start, start + R[r]);
        start += R[r];
    }
}

void complete_levels_directed(edges_t& g, const ranks_t& R) {
    int start = 0, ranks = R.size();
    for (int r = 0; r < ranks; r++) {
        add_all_edges_bidirectional(g, start, start + R[r]);
        start += R[r];
    }
}

/** Simple graph types like trees and grids with exact, known shape
 */

auto path_undirected(int V) {
    edges_t g;
    for (int u = 0; u + 1 < V; u++)
        g.push_back({u, u + 1});
    return g;
}

auto path_directed(int V) {
    edges_t g;
    for (int u = 0; u + 1 < V; u++)
        g.push_back({u, u + 1});
    return g;
}

auto grid_undirected(int W, int H) {
    edges_t g;
    add_grid_edges(g, W, H, 0, 1);
    add_grid_edges(g, W, H, 1, 0);
    return g;
}

auto grid_directed(int W, int H) {
    edges_t g;
    add_grid_edges(g, W, H, 0, 1);
    add_grid_edges(g, W, H, 1, 0);
    return g;
}

auto circular_grid_undirected(int W, int H) {
    edges_t g;
    add_circular_grid_edges(g, W, H, 0, 1);
    add_circular_grid_edges(g, W, H, 1, 0);
    return g;
}

auto circular_grid_directed(int W, int H) {
    edges_t g;
    add_circular_grid_edges(g, W, H, 0, 1);
    add_circular_grid_edges(g, W, H, 1, 0);
    return g;
}

auto grid3_undirected(int X, int Y, int Z) {
    edges_t g;
    add_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

auto grid3_directed(int X, int Y, int Z) {
    edges_t g;
    add_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

auto circular_grid3_undirected(int X, int Y, int Z) {
    edges_t g;
    add_circular_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_circular_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_circular_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

auto circular_grid3_directed(int X, int Y, int Z) {
    edges_t g;
    add_circular_grid3_edges(g, X, Y, Z, 0, 0, 1);
    add_circular_grid3_edges(g, X, Y, Z, 0, 1, 0);
    add_circular_grid3_edges(g, X, Y, Z, 1, 0, 0);
    return g;
}

auto complete_undirected(int V) {
    edges_t g;
    add_all_edges_undirected(g, 0, V);
    return g;
}

auto complete_directed(int V) {
    edges_t g;
    add_all_edges_directed(g, 0, V);
    return g;
}

auto cycle_undirected(int V) {
    edges_t g;
    for (int u = 1; u < V; u++) {
        g.push_back({u - 1, u});
    }
    if (V >= 3)
        g.push_back({V - 1, 0});
    return g;
}

auto cycle_directed(int V) {
    edges_t g;
    for (int u = 1; u < V; u++) {
        g.push_back({u - 1, u});
    }
    if (V >= 3)
        g.push_back({V - 1, 0});
    return g;
}

auto regular_ring(int V, int k) {
    assert(k >= 2 && k % 2 == 0 && k < V);
    edges_t g;
    add_regular_ring_lattice(g, 0, V, k);
    return g;
}

auto ary_tree_undirected(int r, int V) {
    edges_t g;
    for (int i = 1; i < V; i++)
        g.push_back({i, (i - 1) / r});
    return g;
}

auto ary_tree_directed(int r, int V, bool toparent = true, bool tochild = false) {
    edges_t g;
    if (toparent)
        for (int i = 1; i < V; i++)
            g.push_back({i, (i - 1) / r});
    if (tochild)
        for (int i = 1; i < V; i++)
            g.push_back({(i - 1) / r, i});
    return g;
}

auto perfect_binary_tree_undirected(int h) {
    int V = (1 << h) - 1;
    return ary_tree_undirected(2, V);
}

auto perfect_binary_tree_directed(int h, bool toparent = true, bool tochild = false) {
    int V = (1 << h) - 1;
    return ary_tree_directed(2, V, toparent, tochild);
}

auto perfect_tree_undirected(int r, int h) {
    int V = r == 1 ? h + 1 : (intpow(r, h + 1) - 1) / (r - 1);
    return ary_tree_undirected(r, V);
}

auto perfect_tree_directed(int r, int h, bool toparent = true, bool tochild = false) {
    int V = r == 1 ? h + 1 : (intpow(r, h + 1) - 1) / (r - 1);
    return ary_tree_directed(r, V, toparent, tochild);
}

auto complete_multipartite(const ranks_t& R) {
    int V = accumulate(begin(R), end(R), 0);
    int start = 0, ranks = R.size();
    edges_t g;
    for (int r = 0; r < ranks; r++) {
        int mid = start + R[r];
        add_level_step_full(g, start, mid, mid, V);
        start = mid;
    }
    return g;
}

auto complete_bipartite(int U, int V) {
    edges_t g;
    add_all_edges_bipartite(g, 0, U, 0, V);
    return g;
}

/** More complex graphs with known shape
 */

auto johnson(int n, int k) {
    assert(0 < n && n <= 31 && 0 < k && k <= n);
    int V = choose(n, k);
    edges_t g;

    unordered_map<int, int> label;
    int l = 0, fmask = (1 << n) - 1;
    FOR_EACH_MASK (u, k, n) { label[u] = l++; }
    (void)V, assert(l == V);

    FOR_EACH_MASK (u, k, n)
        FOR_EACH_BIT (u, a, i)
            FOR_EACH_BIT ((~u & fmask), b, j)
                if (a < b)
                    g.push_back({label[u], label[(u ^ i) | j]});
    assert(verify_edges_undirected(g, V, k * (n - k) * V / 2));
    return g;
}

auto kneser(int n, int k) {
    assert(0 < n && n <= 31 && 0 < k && k <= n);
    int V = choose(n, k);
    edges_t g;

    unordered_map<int, int> label;
    int l = 0;
    FOR_EACH_MASK (u, k, n) { label[u] = l++; }
    (void)V, assert(l == V);

    // TODO: improve complexity
    FOR_EACH_MASK (u, k, n)
        FOR_EACH_MASK (v, k, n)
            if (!(u & v) && u < v)
                g.push_back({label[u], label[v]});
    assert(verify_edges_undirected(g, V, V * choose(n - k, k) / 2));
    return g;
}

auto peterson() {
    // special case of kneser graph
    return kneser(5, 2);
}

auto star(int n) {
    edges_t g;
    add_level_step_full(g, 0, 1, 1, n);
    return g;
}

auto stacked_book(int m, int n) {
    int k = (m + 1) * n;
    edges_t g;
    for (int i = 0; i < k; i += m + 1)
        add_level_step_full(g, i, i + 1, i + 1, i + m + 1);
    for (int i = m + 1; i < k; i++)
        g.push_back({i - (m + 1), i});
    return g;
}

auto book(int m) {
    // special case of stacked book
    return stacked_book(m, 2);
}

auto wheel(int n) {
    assert(n >= 3);
    edges_t g;
    for (int u = 0; u < n; u++) {
        g.push_back({u, n});
        g.push_back({u, u + 1 == n ? 0 : u + 1});
    }
    return g;
}

auto cogwheel(int n) {
    assert(n >= 3);
    edges_t g;
    for (int u = 0; u < n; u++) {
        g.push_back({2 * u, 2 * n});
        g.push_back({2 * u, 2 * u + 1});
        g.push_back({2 * u, u == 0 ? 2 * n - 1 : 2 * u - 1});
    }
    return g;
}

auto web(int n, int h) {
    edges_t g;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < n; j++) {
            int u = i * n + j;
            g.push_back({u, j + 1 == n ? i * n : u + 1});
            if (i + 1 < h)
                g.push_back({u, u + n});
        }
    }
    return g;
}

auto turan(int n, int r) {
    ranks_t R(n);
    for (int i = 0; i < r; i++)
        R[i] = (n + i) / r;
    return complete_multipartite(R);
}

auto circulant(int V, const offsets_t& O) {
    edges_t g;
    for (int o : O)
        add_circulant_arcs(g, 0, V, o);
    return g;
}

auto sudoku(int n) {
    int m = int(sqrt(n));
    assert(m * m == n);
    edges_t g;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int u = i * n + j;
            // same col, different row
            for (int k = i + 1; k < n; k++)
                g.push_back({u, k * n + j});
            // same row, different col
            for (int k = j + 1; k < n; k++)
                g.push_back({u, i * n + k});
            // same cell, different row, different col
            int x = i / m, y = j / m;
            for (int a = x * m; a < (x + 1) * m; a++) {
                for (int b = y * m; b < (y + 1) * m; b++) {
                    int v = a * n + b;
                    if (a != i && b != j && u < v)
                        g.push_back({u, v});
                }
            }
        }
    }
    assert(verify_edges_undirected(g, n * n, n * n * (3 * n - 2 * m - 1) / 2));
    return g;
}

/** Randomized known shape graphs
 */

auto random_tree_undirected(int V) {
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V);
    return g;
}

auto random_tree_directed(int V, bool toparent = true, bool tochild = false) {
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V, toparent, tochild);
    return g;
}

auto random_regular(int V, int k) {
    // Just sample
    return regular_sample(V, k);
}

auto random_regular_connected(int V, int k) {
    edges_t g;
    do {
        g = random_regular(V, k);
    } while (!is_connected_undirected(g, V));
    return g;
}

auto random_regular_directed(int V, int k) {
    // Just sample
    return regular_directed_sample(V, k);
}

auto random_regular_directed_connected(int V, int k) {
    edges_t g;
    do {
        g = random_regular_directed(V, k);
    } while (!is_connected_directed(g, V));
    return g;
}

auto random_regular_bipartite(int U, int V, int k) {
    return regular_bipartite_sample(U, V, k);
}

/** Completely random and not necessarily connected graphs (Erdős–Rényi graphs)
 */

auto random_uniform_undirected(int V, double p) {
    edges_t g;
    add_uniform_edges_undirected(g, 0, V, min(1.0, p));
    return g;
}

auto random_uniform_directed(int V, double p) {
    edges_t g;
    add_uniform_edges_directed(g, 0, V, min(1.0, p));
    return g;
}

auto random_uniform_bipartite(int U, int V, double p) {
    edges_t g;
    add_uniform_edges_bipartite(g, 0, U, 0, V, min(1.0, p));
    return g;
}

auto random_exact_undirected(int V, int E) {
    assert(E <= 1L * V * (V - 1) / 2);
    return choose_sample(E, 0, V - 1, false);
}

auto random_exact_directed(int V, int E) {
    assert(E <= 1L * V * (V - 1));
    edges_t g, f = pair_sample(E + V, 0, V - 1, 0, V - 1);
    shuffle(begin(f), end(f), mt);
    add_nontrivial_edges_directed(g, f, E);
    return g;
}

auto random_exact_bipartite(int U, int V, int E) {
    assert(E <= 1L * V * (V - 1));
    return pair_sample(E, 0, U - 1, 0, V - 1);
}

/** Completely random but connected graphs (Erdős–Rényi with bootstrap tree)
 */

auto random_uniform_undirected_connected(int V, double p) {
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V);
    p = min(p, 1.0);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.push_back({u, v});
    }
    return g;
}

auto random_uniform_rooted_dag_connected(int V, double p) {
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V, false, true);
    p = min(p, 1.0);
    for (int v = 1; v < V; v++) {
        binomd distk(v, p);
        for (int u : int_sample(distk(mt), 0, v - 1))
            if (u != parent[v])
                g.push_back({u, v});
    }
    return g;
}

auto random_exact_undirected_connected(int V, int E) {
    assert(V - 1 <= E && E <= 1L * V * (V - 1) / 2);
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V);
    if (E == V - 1)
        return g;

    int k = min(1L * V * (V - 1) / 2, 0L + E + V);
    auto f = choose_sample(k, 0, V - 1, false);
    shuffle(begin(f), end(f), mt);
    add_edges_except_undirected(g, f, parent, E - (V - 1));
    return g;
}

auto random_exact_rooted_dag_connected(int V, int E) {
    assert(V - 1 <= E && E <= 1L * V * (V - 1) / 2);
    edges_t g;

    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V, false, true);
    if (E == V - 1)
        return g;

    int k = min(1L * V * (V - 1) / 2, 0L + E + V);
    auto f = choose_sample(k, 0, V - 1, false);
    shuffle(begin(f), end(f), mt);
    add_edges_except_directed(g, f, parent, E - (V - 1));
    return g;
}

/** Many complete graphs joined in different ways
 */

auto disjoint_complete_undirected(int n, int k) {
    edges_t g;
    for (int i = 0; i < n; i++)
        add_all_edges_undirected(g, i * k, (i + 1) * k);
    return g;
}

auto disjoint_complete_directed(int n, int k) {
    edges_t g;
    for (int i = 0; i < n; i++)
        add_all_edges_directed(g, i * k, (i + 1) * k);
    return g;
}

auto one_connected_complete_undirected(int n, int k) {
    auto g = disjoint_complete_undirected(n, k);
    for (int i = 0; i + 1 < n; i++) {
        int u = k * i + (i % k), v = k * (i + 1) + (i % k);
        g.push_back({u, v});
    }
    return g;
}

auto one_connected_complete_directed(int n, int k) {
    auto g = disjoint_complete_directed(n, k);
    for (int i = 0; i + 1 < n; i++) {
        int u = k * i + (i % k), v = k * (i + 1) + (i % k);
        g.push_back({u, v});
    }
    return g;
}

auto k_connected_complete_undirected(int n, int k) {
    auto g = disjoint_complete_undirected(n, k);
    for (int i = 0; i + 1 < n; i++)
        for (int j = 0; j < k; j++)
            g.push_back({k * i + j, k * (i + 1) + j});
    return g;
}

auto k_connected_complete_directed(int n, int k) {
    auto g = disjoint_complete_directed(n, k);
    for (int i = 0; i + 1 < n; i++)
        for (int j = 0; j < k; j++)
            g.push_back({k * i + j, k * (i + 1) + j});
    return g;
}

/** Level (ranked) connected graphs, optionally looped
 *
 * Each level is connected (full,uniform) to the next level.
 */

auto random_full_level(int V, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels_undirected(g, R);
    return g;
}

auto random_full_level_dag(int V, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels_directed(g, R);
    return g;
}

auto random_full_level_flow(int V, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_full(g, R, loop);
    complete_levels_directed(g, R);
    return g;
}

auto random_uniform_level(int V, double p, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop);
    complete_levels_undirected(g, R);
    return g;
}

auto random_uniform_level_dag(int V, double p, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    complete_levels_directed(g, R);
    return g;
}

auto random_uniform_level_flow(int V, double p, int ranks, int m = 1, bool loop = false) {
    edges_t g;
    auto R = partition_sample_flow(V, ranks, m);
    link_levels_uniform(g, p, R, loop, true, true);
    complete_levels_directed(g, R);
    return g;
}

// ***** Convert to flow network (temporary)

// flow_graph random_flow_graph(int V, double p, long max_cap) {
//     intd rankd(3, max(3, V / 3));
//     int ranks = rankd(mt), m = min(7, V / ranks);
//     p = min(1.0, p);
//     digraph g = random_uniform_level_flow(V, p, ranks, m, false);
//     return add_caps(g, max_cap);
// }

#endif // GRAPH_GENERATOR_HPP
