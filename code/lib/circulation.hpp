#ifndef CIRCULATION_HPP
#define CIRCULATION_HPP

#include "graph_generator.hpp"

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

struct circulation_network {
    edges_t g;
    circulation_network_kind kind;
    int V, E, S;
    int X, Y, Z, k, levels, min_width;
    double p;
    vector<long> supply, cap;
    vector<long> cost;
};

// S arbitrates network size/complexity.
auto generate_circulation_network(circulation_network_kind i, int S) {
    assert(50 <= S && S <= 50000);

    static auto rs = [](int x, double e) -> int { return ceil(pow(x, e)); };
    static auto dV = [](int x, double e) -> int {
        int n = max(3, rs(x, e)), s = sqrt(n);
        return intd(n, n + s)(mt);
    };
    static auto roughly = [](int x, double lo, double hi, int min, int max) -> int {
        return clamp(intd(rs(x, lo), rs(x, hi))(mt), min, max);
    };

    edges_t g;
    int V, X = 0, Y = 0, Z = 0, k = 0, levels = 0, min_width = 0;
    double p = 0.0;
    vector<int> ints;

    switch (i) {
    case CN_UNIFORM_SPARSE:
        V = dV(S, 0.86), p = reald(2.5, 3.5)(mt) / V;
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_MODERATE:
        V = dV(S, 0.75), p = reald(5.0, 6.0)(mt) / sqrt(V);
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_DENSE:
        V = dV(S, 0.72), p = reald(0.50, 0.95)(mt);
        g = random_uniform_directed_connected(V, p);
        break;
    case CN_UNIFORM_LEVEL_LONG_SPARSE:
        V = dV(S, 0.82), p = reald(8.0, 10.0)(mt) / V;
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = dV(S, 0.74), p = reald(8.0, 20.0)(mt) / V;
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_MODERATE:
        V = dV(S, 0.82), p = reald(5.0, 6.0)(mt) / sqrt(V);
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = dV(S, 0.74), p = reald(5.0, 9.0)(mt) / sqrt(V);
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_UNIFORM_LEVEL_LONG_DENSE:
        V = dV(S, 0.81), p = reald(0.7, 0.95)(mt);
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_UNIFORM_LEVEL_WIDE_DENSE:
        V = dV(S, 0.74), p = reald(0.7, 0.85)(mt);
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width, true);
        break;
    case CN_COMPLETE:
        V = dV(S, 0.72);
        g = complete_directed(V), add_reverse_edges(g);
        break;
    case CN_CIRCULAR_GRID2:
        X = dV(S, 0.435), Y = dV(S, 0.435), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case CN_CIRCULAR_GRID3:
        X = dV(S, 0.280), Y = dV(S, 0.280), Z = dV(S, 0.280), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case CN_CIRCULANT:
        V = dV(S, 0.79), k = clamp(dV(S, 0.38), 2, V - 2);
        ints = int_sample(k, 2, V), ints.push_back(1);
        g = circulant(V, ints);
        break;
    case CN_MULTIPARTITE:
        V = dV(S, 0.72), k = dV(S, 0.33);
        ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse_edges(g);
        break;
    default:
        throw runtime_error("Invalid circulation network kind");
    }
    assert(verify_edges_directed(g, V) && is_connected_directed(g, V));

    int E = g.size();
    circulation_network cn;
    cn.kind = i;
    cn.V = V, cn.E = E, cn.S = S;
    cn.X = X, cn.Y = Y, cn.Z = Z, cn.levels = levels, cn.min_width = min_width;
    cn.k = k, cn.p = p;
    return cn;
}

auto add_supply_circulation_network(circulation_network& cn, long minsupply,
                                    long maxsupply) {
    cn.cap = int_gen<long>(cn.E, minsupply, maxsupply);
}
auto add_supply_circulation_network(circulation_network& cn, long maxsupply) {
    return add_supply_circulation_network(cn, 1, maxsupply);
}
auto add_cap_circulation_network(circulation_network& cn, long mincap, long maxcap) {
    cn.cap = int_gen<long>(cn.E, mincap, maxcap);
}
auto add_cap_circulation_network(circulation_network& cn, long maxcap) {
    return add_cap_circulation_network(cn, 1, maxcap);
}
auto add_cost_circulation_network(circulation_network& cn, long mincost, long maxcost) {
    cn.cap = int_gen<long>(cn.E, mincost, maxcost);
}
auto add_cost_circulation_network(circulation_network& cn, long maxcost) {
    return add_cost_circulation_network(cn, 1, maxcost);
}

#endif // CIRCULATION_HPP
