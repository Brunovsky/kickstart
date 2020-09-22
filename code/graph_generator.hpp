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

void add_reverse_edges(edges_t& g) {
    int E = g.size();
    g.reserve(2 * E);
    for (int e = 0; e < E; e++) {
        auto [u, v] = g[e];
        g.push_back({v, u});
    }
}

void add_uniform_self_loops(edges_t& g, int u1, int u2, double p) {
    int V = u2 - u1;
    if (V >= 10 && p <= 0.25) {
        binomd distk(V, p);
        for (int u : int_sample(distk(mt), u1, u2 - 1))
            g.push_back({u, u});
    } else {
        boold distp(p);
        for (int u = u1; u < u2; u++)
            if (distp(mt))
                g.push_back({u, u});
    }
}

void add_uniform_edges_undirected(edges_t& g, int u1, int u2, double p) {
    int V = u2 - u1;
    if (V >= 10 && p <= 0.25) {
        longbinomd distk(1L * V * (V - 1) / 2, p);
        for (auto [u, v] : choose_sample(distk(mt), u1, u2 - 1, false))
            g.push_back({u, v});
    } else {
        boold distp(p);
        for (int u = u1 + 1; u < u2; u++)
            for (int v = u1; v < u; v++)
                if (distp(mt))
                    g.push_back({u, v});
    }
}

void add_uniform_edges_directed(edges_t& g, int u1, int u2, double p) {
    int V = u2 - u1;
    if (V >= 10 && p <= 0.25) {
        longbinomd distk(1L * V * (V - 1), p);
        for (auto [u, v] : pair_sample(distk(mt), u1, u2 - 1, u1, u2 - 1))
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
    int U = u2 - u1, V = v2 - v1;
    if (1L * U * V >= 100 && p <= 0.25) {
        longbinomd distk(1L * U * V, p);
        for (auto [u, v] : pair_sample(distk(mt), u1, u2 - 1, v1, v2 - 1))
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

void add_edges_missing_undirected(edges_t& g, const edges_t& edges, int S) {
    auto adj = make_adjacency_set_undirected(g);
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && !adj.count({u, v}) && !adj.count({v, u}))
            g.push_back({u, v}), S--;
    }
    assert(S == 0);
}

void add_edges_missing_directed(edges_t& g, const edges_t& edges, int S) {
    auto adj = make_adjacency_set_directed(g);
    for (auto [u, v] : edges) {
        if (S == 0)
            break;
        if (u != v && !adj.count({u, v}))
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

void remove_uniform(edges_t& g, double p) {
    edges_t f;
    boold distp(p);
    for (auto [u, v] : g)
        if (!distp(mt))
            f.push_back({u, v});
    g = move(f);
}

void add_tarjan_back_edges(edges_t& g, int V, int s = 0) {
    adjacency_lists_t adj = make_adjacency_lists_directed(g, V);
    vector<int> start(V, -1), lowlink(V, -1), rev(V);
    int time = 0;
    function<void(int)> dfs = [&](int u) -> void {
        rev[time] = u, start[u] = lowlink[u] = time++;
        for (int v : adj[u]) {
            if (start[v] == -1) {
                dfs(v), lowlink[u] = min(lowlink[u], lowlink[v]);
            } else {
                lowlink[u] = min(lowlink[u], start[v]);
            }
        }
        if (start[u] > 0 && start[u] == lowlink[u]) {
            int x = intd(start[u], time - 1)(mt);
            int y = intd(0, start[u] - 1)(mt);
            int v = rev[x], w = rev[y];
            adj[v].push_back(w), g.push_back({v, w});
            lowlink[u] = y;
        }
    };
    dfs(s);
}

void add_uniform_missing_undirected(edges_t& g, int V, double p) {
    auto adj = make_adjacency_set_undirected(g);
    longbinomd dist(1L * V * (V - 1) / 2, p);
    for (auto [u, v] : choose_sample(dist(mt), 0, V - 1, false))
        if (!adj.count({u, v}) && !adj.count({v, u}))
            g.push_back({u, v});
}

void add_uniform_missing_directed(edges_t& g, int V, double p) {
    auto adj = make_adjacency_set_directed(g);
    longbinomd dist(1L * V * (V - 1), p);
    for (auto [u, v] : pair_sample(dist(mt), 0, V - 1, 0, V - 1))
        if (u != v && !adj.count({u, v}))
            g.push_back({u, v});
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
    assert((dx || dy) && 0 <= dx && dx < X && 0 <= dy && dy < Y);
    longbinomd distk((X - dx) * (Y - dy), p);
    for (auto [x, y] : pair_sample(distk(mt), 0, X - dx - 1, 0, Y - dy - 1))
        g.push_back({calc_grid2(X, x, y), calc_grid2(X, x + dx, y + dy)});
}

void add_uniform_circular_grid_edges(edges_t& g, int X, int Y, double p, int dx, int dy) {
    assert((dx || dy) && 0 <= dx && dx < X && 0 <= dy && dy < Y);
    longbinomd distk(1L * X * Y, p);
    for (auto [x, y] : pair_sample(distk(mt), 0, X - 1, 0, Y - 1))
        g.push_back({calc_grid2(X, x, y), calc_grid2(X, (x + dx) % X, (y + dy) % Y)});
}

void add_uniform_grid3_edges(edges_t& g, int X, int Y, int Z, double p, int dx, int dy,
                             int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx < X && 0 <= dy && dy < Y && 0 <= dz && dz < Z);
    auto add = [&](int x, int y, int z) {
        g.push_back(
            {calc_grid3(X, Y, x, y, z), calc_grid3(X, Y, x + dx, y + dy, z + dz)});
    };
    if (1L * (X - dx) * (Y - dy) * (Z - dz) < 1000 || p > 0.25) {
        boold distp(p);
        for (int x = 0; x + dx < X; x++)
            for (int y = 0; y + dy < Y; y++)
                for (int z = 0; z + dz < Z; z++)
                    if (distp(mt))
                        add(x, y, z);
    } else if (X - dx == min(X - dx, min(Y - dy, Z - dz))) {
        longbinomd dist(1L * (Y - dy) * (Z - dz), p);
        for (int x = 0; x + dx < X; x++)
            for (auto [y, z] : pair_sample(dist(mt), 0, Y - dy - 1, 0, Z - dz - 1))
                add(x, y, z);
    } else if (Y - dy == min(X - dx, min(Y - dy, Z - dz))) {
        longbinomd dist(1L * (X - dx) * (Z - dz), p);
        for (int y = 0; y + dy < Y; y++)
            for (auto [x, z] : pair_sample(dist(mt), 0, X - dx - 1, 0, Z - dz - 1))
                add(x, y, z);
    } else {
        longbinomd dist(1L * (X - dx) * (Y - dy), p);
        for (int z = 0; z + dz < Z; z++)
            for (auto [x, y] : pair_sample(dist(mt), 0, X - dx - 1, 0, Y - dy - 1))
                add(x, y, z);
    }
}

void add_uniform_circular_grid3_edges(edges_t& g, int X, int Y, int Z, double p, int dx,
                                      int dy, int dz) {
    assert(dx || dy || dz);
    assert(0 <= dx && dx < X && 0 <= dy && dy < Y && 0 <= dz && dz < Z);
    auto add = [&](int x, int y, int z) {
        g.push_back({calc_grid3(X, Y, x, y, z),
                     calc_grid3(X, Y, (x + dx) % X, (y + dy) % Y, (z + dz) % Z)});
    };
    if (1L * X * Y * Z < 1000 || p > 0.25) {
        boold distp(p);
        for (int x = 0; x < X; x++)
            for (int y = 0; y < Y; y++)
                for (int z = 0; z < Z; z++)
                    if (distp(mt))
                        add(x, y, z);
    } else if (X == min(X, min(Y, Z))) {
        longbinomd dist(1L * Y * Z, p);
        for (int x = 0; x < X; x++)
            for (auto [y, z] : pair_sample(dist(mt), 0, Y - 1, 0, Z - 1))
                add(x, y, z);
    } else if (Y == min(X, min(Y, Z))) {
        longbinomd dist(1L * X * Z, p);
        for (int y = 0; y < Y; y++)
            for (auto [x, z] : pair_sample(dist(mt), 0, X - 1, 0, Z - 1))
                add(x, y, z);
    } else {
        longbinomd dist(1L * X * Y, p);
        for (int z = 0; z < Z; z++)
            for (auto [x, y] : pair_sample(dist(mt), 0, X - 1, 0, Y - 1))
                add(x, y, z);
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

    int U = u2 - u1, V = v2 - v1;
    vector<bool> out(U, false), in(V, false);
    longbinomd distk(1L * U * V, p);
    auto sample = pair_sample(distk(mt), u1, u2 - 1, v1, v2 - 1);
    for (auto [u, v] : sample)
        g.push_back({u, v}), out[u - u1] = in[v - v1] = true;

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

auto random_uniform_directed_connected(int V, double p) {
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V, false, true);
    add_tarjan_back_edges(g, V);
    assert(is_connected_directed(g, V));
    add_uniform_missing_directed(g, V, p);
    return g;
}

auto random_exact_undirected_connected(int V, int E) {
    assert(V - 1 <= E && E <= 1L * V * (V - 1) / 2);
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V);
    if (E == g.size())
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
    if (E == g.size())
        return g;

    int k = min(1L * V * (V - 1) / 2, 0L + E + V);
    auto f = choose_sample(k, 0, V - 1, false);
    shuffle(begin(f), end(f), mt);
    add_edges_except_directed(g, f, parent, E - (V - 1));
    return g;
}

auto random_exact_directed_connected(int V, int E) {
    assert(2 * V - 2 <= E && E <= 1L * V * (V - 1));
    edges_t g;
    auto parent = parent_sample(V);
    add_parent_edges(g, parent, 1, V, false, true);
    add_tarjan_back_edges(g, V);
    assert(is_connected_directed(g, V));
    assert(g.size() <= E);
    if (E == g.size())
        return g;

    int n = g.size(), k = min(1L * V * V, 0L + E + V);
    auto f = pair_sample(k, 0, V - 1, 0, V - 1);
    shuffle(begin(f), end(f), mt);
    add_edges_missing_directed(g, f, E - n);
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

/** Flow networks
 */

struct flow_network {
    int V, E, s, t;
    edges_t g;
};

struct cap_flow_network : flow_network {
    flows_t cap;
};

struct cap_cost_flow_network : cap_flow_network {
    costs_t cost;
};

enum flow_network_kind : int {
    FN_UNIFORM_SPARSE = 0,
    FN_UNIFORM_MODERATE = 1,
    FN_UNIFORM_DENSE = 2,
    FN_UNIFORM_LEVEL_LONG_SPARSE = 3,
    FN_UNIFORM_LEVEL_WIDE_SPARSE = 4,
    FN_UNIFORM_LEVEL_LONG_MODERATE = 5,
    FN_UNIFORM_LEVEL_WIDE_MODERATE = 6,
    FN_UNIFORM_LEVEL_LONG_DENSE = 7,
    FN_UNIFORM_LEVEL_WIDE_DENSE = 8,
    FN_COMPLETE = 9,
    FN_NORMAL_GRID2 = 10,
    FN_CIRCULAR_GRID2 = 11,
    FN_NORMAL_GRID3 = 12,
    FN_CIRCULAR_GRID3 = 13,
    FN_KCONNECTED_COMPLETE = 14,
    FN_END = 15,
};

string flow_kind_name[] = {
    "random uniform sparse",
    "random uniform moderate",
    "random uniform dense",
    "uniform level long sparse",
    "uniform level wide sparse",
    "uniform level long moderate",
    "uniform level wide moderate",
    "uniform level long dense",
    "uniform level wide dense",
    "complete",
    "normal 2d grid",
    "circular 2d grid",
    "normal 3d grid",
    "circular 3d grid",
    "k-connected complete",
    "---",
};

// S arbitrates network size/complexity.
auto generate_flow_network(flow_network_kind i, int S) {
    assert(10 <= S && S <= 50000);
    const int a = int(pow(S, 0.90)), b = int(pow(S, 0.75)), c = int(pow(S, 0.68));
    const int d = int(pow(S, 0.57)), s = int(pow(S, 0.50)), f = int(pow(S, 0.40));
    const int t = int(pow(S, 0.37)), u = int(pow(S, 0.30)), v = int(pow(S, 0.20));

    intd Sd(S, S + s), ad(a, a + s), bd(b, b + f), cd(c, c + f), dd(d, d + u);
    intd sd(s, s + v), fd(f, f + v), td(t, t + v), ud(u, u + v);
    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    edges_t g;
    int V = -1, ranks, m, X, Y, Z, k;
    double p;

    const auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    const auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    const auto dense = [&]() { return densed(mt); };
    const auto longlevel = [&]() { return min(V, intd(20, max(20, V / 5))(mt)); };
    const auto widelevel = [&]() { return min(V, intd(5, clamp(V, 5, 12))(mt)); };
    const auto longm = [&]() { return min(8, V / ranks); };
    const auto widem = [&]() { return min(20, V / ranks); };

    switch (i) {
    case FN_UNIFORM_SPARSE:
        V = Sd(mt), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_MODERATE:
        V = bd(mt), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_DENSE:
        V = cd(mt), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_LEVEL_LONG_SPARSE:
        V = ad(mt), p = sparse(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = bd(mt), p = sparse(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_UNIFORM_LEVEL_LONG_MODERATE:
        V = ad(mt), p = moderate(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = bd(mt), p = moderate(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_UNIFORM_LEVEL_LONG_DENSE:
        V = ad(mt), p = dense(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_DENSE:
        V = bd(mt), p = dense(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case FN_COMPLETE:
        V = cd(mt);
        g = complete_directed(V);
        break;
    case FN_NORMAL_GRID2:
        X = dd(mt), Y = dd(mt), V = X * Y;
        g = grid_directed(X, Y);
        break;
    case FN_CIRCULAR_GRID2:
        X = dd(mt), Y = dd(mt), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case FN_NORMAL_GRID3:
        X = td(mt), Y = td(mt), Z = td(mt), V = X * Y * Z;
        g = grid3_directed(X, Y, Z);
        break;
    case FN_CIRCULAR_GRID3:
        X = td(mt), Y = td(mt), Z = td(mt), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case FN_KCONNECTED_COMPLETE:
        m = cd(mt), k = ud(mt), V = m * k;
        g = k_connected_complete_directed(m, k);
        break;
    default:
        throw runtime_error("Invalid flow network kind");
    }
    int E = g.size();
    return flow_network{V, E, 0, V - 1, g};
}

auto generate_cap_flow_network(flow_network_kind i, int S, flow_t maxcap,
                               flow_t mincap = 1) {
    auto fn = generate_flow_network(i, S);
    auto cap = int_gen<flow_t>(fn.E, mincap, maxcap);
    return cap_flow_network{fn, cap};
}

auto generate_cap_cost_flow_network(flow_network_kind i, int S, flow_t maxcap,
                                    cost_t maxcost, flow_t mincap = 1,
                                    cost_t mincost = 1) {
    auto fn = generate_flow_network(i, S);
    auto cap = int_gen<flow_t>(fn.E, mincap, maxcap);
    auto cost = int_gen<cost_t>(fn.E, mincost, maxcost);
    return cap_cost_flow_network{{fn, cap}, cost};
}

/** Circulation networks
 */

struct circulation_network {
    int V, E;
    edges_t g;
};

struct supply_circulation_network : circulation_network {
    flows_t supply, cap;
    costs_t cost;
};

enum circulation_network_kind : int {
    CN_UNIFORM_SPARSE = 0,
    CN_UNIFORM_MODERATE = 1,
    CN_UNIFORM_DENSE = 2,
    CN_UNIFORM_LEVEL_LONG_SPARSE = 3,
    CN_UNIFORM_LEVEL_WIDE_SPARSE = 4,
    CN_UNIFORM_LEVEL_LONG_MODERATE = 5,
    CN_UNIFORM_LEVEL_WIDE_MODERATE = 6,
    CN_UNIFORM_LEVEL_LONG_DENSE = 7,
    CN_UNIFORM_LEVEL_WIDE_DENSE = 8,
    CN_COMPLETE = 9,
    CN_CIRCULAR_GRID2 = 10,
    CN_CIRCULAR_GRID3 = 11,
    CN_CIRCULANT = 12,
    CN_MULTIPARTITE = 13,
    CN_END = 14,
};

string circulation_kind_name[] = {
    "random uniform sparse",
    "random uniform moderate",
    "random uniform dense",
    "uniform level long sparse",
    "uniform level wide sparse",
    "uniform level long moderate",
    "uniform level wide moderate",
    "uniform level long dense",
    "uniform level wide dense",
    "complete",
    "circular 2d grid",
    "circular 3d grid",
    "circulant",
    "multipartite",
    "---",
};

// S arbitrates network size/complexity.
auto generate_circulation_network(circulation_network_kind i, int S) {
    assert(10 <= S && S <= 50000);
    const int a = int(pow(S, 0.90)), b = int(pow(S, 0.75)), c = int(pow(S, 0.68));
    const int d = int(pow(S, 0.57)), s = int(pow(S, 0.50)), f = int(pow(S, 0.40));
    const int t = int(pow(S, 0.37)), u = int(pow(S, 0.30)), v = int(pow(S, 0.20));

    intd Sd(S, S + s), ad(a, a + s), bd(b, b + f), cd(c, c + f), dd(d, d + u);
    intd sd(s, s + v), fd(f, f + v), td(t, t + v), ud(u, u + v);
    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    edges_t g;
    int V = -1, ranks, m, X, Y, Z, k;
    double p;
    offsets_t ints;

    const auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    const auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    const auto dense = [&]() { return densed(mt); };
    const auto longlevel = [&]() { return min(V, intd(20, max(20, V / 5))(mt)); };
    const auto widelevel = [&]() { return min(V, intd(5, clamp(V, 5, 12))(mt)); };
    const auto longm = [&]() { return min(8, V / ranks); };
    const auto widem = [&]() { return min(20, V / ranks); };
    auto sqrtk = [&]() { return int(sqrt(V) + 1); };

    switch (i) {
    case CN_UNIFORM_SPARSE:
        V = Sd(mt), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_MODERATE:
        V = bd(mt), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_DENSE:
        V = cd(mt), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_LEVEL_LONG_SPARSE:
        V = ad(mt), p = sparse(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = bd(mt), p = sparse(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_MODERATE:
        V = ad(mt), p = moderate(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = bd(mt), p = moderate(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_DENSE:
        V = ad(mt), p = dense(), ranks = longlevel(), m = longm();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_DENSE:
        V = bd(mt), p = dense(), ranks = widelevel(), m = widem();
        g = random_uniform_level_flow(V, p, ranks, m, true);
        break;
    case CN_COMPLETE:
        V = cd(mt);
        g = complete_directed(V), add_reverse_edges(g);
        break;
    case CN_CIRCULAR_GRID2:
        X = dd(mt), Y = dd(mt), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case CN_CIRCULAR_GRID3:
        X = td(mt), Y = td(mt), Z = td(mt), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case CN_CIRCULANT:
        V = bd(mt), k = sqrtk(), ints = int_sample(k, 2, V - 1);
        ints.push_back(1);
        g = circulant(V, ints);
        break;
    case CN_MULTIPARTITE:
        V = cd(mt), k = sqrtk(), ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse_edges(g);
        break;
    default:
        throw runtime_error("Invalid circulation network kind");
    }

    int E = g.size();
    return circulation_network{V, E, g};
}

auto generate_supply_circulation_network(circulation_network_kind i, int S, flow_t sum,
                                         int positives, int negatives, flow_t maxcap = 99,
                                         cost_t maxcost = 99, flow_t mincap = 1,
                                         cost_t mincost = 1) {
    auto fn = generate_circulation_network(i, S);
    auto cap = int_gen<flow_t>(fn.E, mincap, maxcap);
    auto cost = int_gen<cost_t>(fn.E, mincost, maxcost);
    auto supply = supply_sample<flow_t>(fn.E, positives, negatives, sum);
    return supply_circulation_network{fn, cap, cost, supply};
}

/** Distance networks
 */

struct distance_graph {
    int V, E;
    edges_t g;
};

struct weighted_distance_graph : distance_graph {
    costs_t weights;
};

enum distance_graph_kind : int {
    DG_UNIFORM_SPARSE = 0,
    DG_UNIFORM_MODERATE = 1,
    DG_UNIFORM_DENSE = 2,
    DG_COMPLETE = 3,
    DG_MULTIPARTITE = 4,
    DG_DISJOINT_MULTIPARTITE = 5,
    DG_REGULAR_RING = 6,
    DG_NORMAL_GRID2 = 7,
    DG_CIRCULAR_GRID2 = 8,
    DG_NORMAL_GRID3 = 9,
    DG_CIRCULAR_GRID3 = 10,
    DG_END = 11,
};

string distance_kind_name[] = {
    "random uniform sparse", "random uniform moderate",
    "random uniform dense",  "complete",
    "multipartite",          "disjoint multipartite",
    "regular ring",          "normal 2d grid",
    "circular 2d grid",      "normal 3d grid",
    "circular 3d grid",      "---",
};

// S arbitrates network size/complexity.
auto generate_distance_graph(distance_graph_kind i, int S, bool bidirectional = true) {
    assert(10 <= S && S <= 50000);
    const int a = int(pow(S, 0.90)), b = int(pow(S, 0.82)), c = int(pow(S, 0.68));
    const int d = int(pow(S, 0.57)), s = int(pow(S, 0.50)), f = int(pow(S, 0.40));
    const int t = int(pow(S, 0.37)), u = int(pow(S, 0.30)), v = int(pow(S, 0.20));
    const int r = int(pow(S, 0.85));

    intd Sd(S, S + s), ad(a, a + s), bd(b, b + f), cd(c, c + f), dd(d, d + u);
    intd sd(s, s + v), fd(f, f + v), td(t, t + v), ud(u, u + v), rd(r, r + s);
    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    edges_t g;
    int V = -1, X, Y, Z, k, n;
    double p;
    offsets_t ints;

    const auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    const auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    const auto dense = [&]() { return densed(mt); };
    const auto add_reverse = [&]() { bidirectional ? add_reverse_edges(g) : (void)0; };
    auto sqrtk = [&]() { return int(sqrt(V) + 1); };

    switch (i) {
    case DG_UNIFORM_SPARSE:
        V = ad(mt), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_MODERATE:
        V = ad(mt), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_DENSE:
        V = bd(mt), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_COMPLETE:
        V = bd(mt);
        g = complete_directed(V), add_reverse();
        break;
    case DG_MULTIPARTITE:
        V = bd(mt), k = sqrtk(), ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse();
        break;
    case DG_DISJOINT_MULTIPARTITE:
        n = dd(mt), k = fd(mt), V = n * k;
        g = disjoint_complete_directed(n, k), add_reverse();
        break;
    case DG_REGULAR_RING:
        V = rd(mt), k = sqrtk();
        g = regular_ring(V, k), add_reverse();
        break;
    case DG_NORMAL_GRID2:
        X = sd(mt), Y = sd(mt), V = X * Y;
        g = grid_directed(X, Y), add_reverse();
        break;
    case DG_CIRCULAR_GRID2:
        X = sd(mt), Y = sd(mt), V = X * Y;
        g = circular_grid_directed(X, Y), add_reverse();
        break;
    case DG_NORMAL_GRID3:
        X = ud(mt), Y = ud(mt), Z = ud(mt), V = X * Y * Z;
        g = grid3_directed(X, Y, Z), add_reverse();
        break;
    case DG_CIRCULAR_GRID3:
        X = ud(mt), Y = ud(mt), Z = ud(mt), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z), add_reverse();
        break;
    default:
        throw runtime_error("Invalid distance graph kind");
    }

    int E = g.size();
    return distance_graph{V, E, g};
}

auto generate_distance_weighted_graph(distance_graph_kind i, int S, cost_t maxweight,
                                      cost_t minweight, bool bidirectional = true) {
    auto dg = generate_distance_graph(i, S, bidirectional);
    auto weight = int_gen<cost_t>(dg.E, minweight, maxweight);
    return weighted_distance_graph{dg, weight};
}

#endif // GRAPH_GENERATOR_HPP
