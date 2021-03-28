#ifndef GEN_CIRCULATION_HPP
#define GEN_CIRCULATION_HPP

#include "../graph_generator.hpp"

// *****

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
        V = dV(0.75), k = sqrtk(), ints = int_sample(k, 2, V);
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

#endif // GEN_CIRCULATION_HPP
