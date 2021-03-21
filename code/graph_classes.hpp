#ifndef GRAPH_CLASSES_HPP
#define GRAPH_CLASSES_HPP

#include "graph_generator.hpp"

// *****

/** Circulation networks
 */

struct circulation_network {
    edges_t g;
    int V, E, S;
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

    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    auto dV = [&](double e) {
        int n = max(1, int(ceil(pow(S, e)))), s = int(sqrt(n));
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int V = -1, levels, m, X, Y, Z, k;
    double p;
    offsets_t ints;

    auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    auto dense = [&]() { return densed(mt); };
    auto longlevel = [&]() {
        levels = min(V, intd(20, max(20, V / 5))(mt)), m = min(8, V / levels);
    };
    auto widelevel = [&]() {
        levels = min(V, intd(5, clamp(V, 5, 12))(mt)), m = min(20, V / levels);
    };
    auto sqrtk = [&]() { return int(sqrt(V) + 1); };

    switch (i) {
    case CN_UNIFORM_SPARSE:
        V = dV(1.00), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_MODERATE:
        V = dV(0.75), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_DENSE:
        V = dV(0.68), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_LEVEL_LONG_SPARSE:
        V = dV(0.80), p = sparse(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = dV(0.75), p = sparse(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_MODERATE:
        V = dV(0.80), p = moderate(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = dV(0.75), p = moderate(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_DENSE:
        V = dV(0.80), p = dense(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_DENSE:
        V = dV(0.75), p = dense(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m, true);
        break;
    case CN_COMPLETE:
        V = dV(0.68);
        g = complete_directed(V), add_reverse_edges(g);
        break;
    case CN_CIRCULAR_GRID2:
        X = dV(0.57), Y = dV(0.57), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case CN_CIRCULAR_GRID3:
        X = dV(0.37), Y = dV(0.37), Z = dV(0.37), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case CN_CIRCULANT:
        V = dV(0.75), k = sqrtk(), ints = int_sample(k, 2, V - 1);
        ints.push_back(1);
        g = circulant(V, ints);
        break;
    case CN_MULTIPARTITE:
        V = dV(0.68), k = sqrtk(), ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse_edges(g);
        break;
    default:
        throw runtime_error("Invalid circulation network kind");
    }
    assert(verify_edges_directed(g, V) && is_connected_directed(g, V));

    int E = g.size();
    circulation_network cn;
    cn.g = move(g);
    cn.V = V, cn.E = E, cn.S = S;
    return cn;
}

auto add_supply_circulation_network(circulation_network& cn, flow_t minsupply,
                                    flow_t maxsupply) {
    cn.cap = int_gen<flow_t>(cn.E, minsupply, maxsupply);
}
auto add_supply_circulation_network(circulation_network& cn, flow_t maxsupply) {
    return add_supply_circulation_network(cn, 1, maxsupply);
}
auto add_cap_circulation_network(circulation_network& cn, flow_t mincap, flow_t maxcap) {
    cn.cap = int_gen<flow_t>(cn.E, mincap, maxcap);
}
auto add_cap_circulation_network(circulation_network& cn, flow_t maxcap) {
    return add_cap_circulation_network(cn, 1, maxcap);
}
auto add_cost_circulation_network(circulation_network& cn, cost_t mincost,
                                  cost_t maxcost) {
    cn.cap = int_gen<cost_t>(cn.E, mincost, maxcost);
}
auto add_cost_circulation_network(circulation_network& cn, cost_t maxcost) {
    return add_cost_circulation_network(cn, 1, maxcost);
}

/** Distance networks
 */

struct distance_graph {
    edges_t g;
    int V, E, S;
    costs_t weight;
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

    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    auto dV = [&](double e) {
        int n = max(1, int(ceil(pow(S, e)))), s = int(sqrt(n));
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int V = -1, X, Y, Z, k, n;
    double p;
    offsets_t ints;

    auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    auto dense = [&]() { return densed(mt); };
    auto add_reverse = [&]() { bidirectional ? add_reverse_edges(g) : (void)0; };
    auto sqrtk = [&]() { return int(sqrt(V) + 1); };

    switch (i) {
    case DG_UNIFORM_SPARSE:
        V = dV(1.00), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_MODERATE:
        V = dV(0.75), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_DENSE:
        V = dV(0.68), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_COMPLETE:
        V = dV(0.68);
        g = complete_directed(V), add_reverse();
        break;
    case DG_MULTIPARTITE:
        V = dV(0.68), k = sqrtk(), ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse();
        break;
    case DG_DISJOINT_MULTIPARTITE:
        n = dV(0.75), k = dV(0.30), V = n * k;
        g = disjoint_complete_directed(n, k), add_reverse();
        break;
    case DG_REGULAR_RING:
        V = dV(0.85), k = sqrtk();
        g = regular_ring(V, k), add_reverse();
        break;
    case DG_NORMAL_GRID2:
        X = dV(0.57), Y = dV(0.57), V = X * Y;
        g = grid_directed(X, Y), add_reverse();
        break;
    case DG_CIRCULAR_GRID2:
        X = dV(0.57), Y = dV(0.57), V = X * Y;
        g = circular_grid_directed(X, Y), add_reverse();
        break;
    case DG_NORMAL_GRID3:
        X = dV(0.37), Y = dV(0.37), Z = dV(0.37), V = X * Y * Z;
        g = grid3_directed(X, Y, Z), add_reverse();
        break;
    case DG_CIRCULAR_GRID3:
        X = dV(0.37), Y = dV(0.37), Z = dV(0.37), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z), add_reverse();
        break;
    default:
        throw runtime_error("Invalid distance graph kind");
    }
    assert(verify_edges_directed(g, V) && is_connected_directed(g, V));

    int E = g.size();
    distance_graph dg;
    dg.g = move(g);
    dg.V = V, dg.E = E, dg.S = S;
    return dg;
}

auto add_weight_distance_graph(distance_graph& dg, cost_t minweight, cost_t maxweight) {
    dg.weight = int_gen<cost_t>(dg.E, minweight, maxweight);
}
auto add_weight_distance_graph(distance_graph& dg, cost_t maxweight) {
    return add_weight_distance_graph(dg, 1, maxweight);
}

/** Bipartite matching
 */

struct bipartite_graph {
    edges_t g;
    int U, V, E, S;
    costs_t cost;
};

enum bipartite_graph_kind : int {
    BG_UNIFORM_SPARSE = 0,
    BG_UNIFORM_MODERATE = 1,
    BG_UNIFORM_DENSE = 2,
    BG_UNIFORM_SPARSE_IMBALANCED = 3,
    BG_UNIFORM_MODERATE_IMBALANCED = 4,
    BG_UNIFORM_DENSE_IMBALANCED = 5,
    BG_COMPLETE = 6,
    BG_REGULAR = 7,
    BG_END = 8,
};

string bipartite_kind_name[] = {
    "random uniform sparse",
    "random uniform moderate",
    "random uniform dense",
    "complete",
    "regular",
};

// S arbitrates network size/complexity.
auto generate_bipartite_graph(bipartite_graph_kind i, int S) {
    assert(10 <= S && S <= 50000);

    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);
    boold swapd(0.5);

    auto dV = [&](double e) {
        int n = max(1, int(ceil(pow(S, e)))), s = int(sqrt(n));
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int U = -1, V = -1, k;
    double p;
    offsets_t ints;

    auto sqrtUV = [&]() { return sqrt(sqrt(U * V)); };
    auto swapUV = [&]() { swapd(mt) ? swap(U, V) : (void)0; };
    auto sparse = [&]() { return min(1.0, sparsed(mt) / sqrt(U * V)); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrtUV()); };
    auto dense = [&]() { return densed(mt); };
    auto sqrtk = [&]() { return int(sqrtUV() + 1); };

    switch (i) {
    case BG_UNIFORM_SPARSE:
        U = dV(1.00), V = dV(1.00), p = sparse();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE:
        U = dV(0.75), V = dV(0.75), p = moderate();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE:
        U = dV(0.68), V = dV(0.68), p = dense();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_SPARSE_IMBALANCED:
        U = dV(0.75), V = dV(1.15), p = sparse(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE_IMBALANCED:
        U = dV(0.56), V = dV(0.86), p = moderate(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE_IMBALANCED:
        U = dV(0.51), V = dV(0.78), p = dense(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_COMPLETE:
        U = dV(0.68), V = dV(0.68);
        g = complete_bipartite(U, V);
        break;
    case BG_REGULAR:
        U = dV(0.85), V = dV(0.85), k = sqrtk();
        g = random_regular_bipartite(U, V, k);
        break;
    default:
        throw runtime_error("Invalid bipartite graph kind");
    }

    int E = g.size();
    bipartite_graph bg;
    bg.g = move(g);
    bg.U = U, bg.V = V, bg.E = E, bg.S = S;
    return bg;
}

auto add_cost_bipartite_graph(bipartite_graph& bg, cost_t mincost, cost_t maxcost) {
    bg.cost = int_gen<cost_t>(bg.E, mincost, maxcost);
}
auto add_cost_bipartite_graph(bipartite_graph& bg, cost_t maxcost) {
    return add_cost_bipartite_graph(bg, 1, maxcost);
}

#endif // GRAPH_CLASSES_HPP
