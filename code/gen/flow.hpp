#ifndef GEN_FLOW_HPP
#define GEN_FLOW_HPP

#include "../graph_generator.hpp"

// *****

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

struct flow_network {
    edges_t g;
    flow_network_kind kind;
    int V, E, S, s, t;
    int X, Y, Z, n, k, levels, min_width;
    double p;
    vector<long> cap;  // edge capacities
    vector<long> cost; // edge costs
};

// S arbitrates network size/complexity.
auto generate_flow_network(flow_network_kind i, int S) {
    assert(50 <= S && S <= 100'000);

    static auto rs = [](int x, double e) -> int { return ceil(pow(x, e)); };
    static auto dV = [](int x, double e) -> int {
        int n = max(3, rs(x, e)), s = sqrt(n);
        return intd(n, n + s)(mt);
    };
    static auto roughly = [](int x, double lo, double hi, int min, int max) -> int {
        return clamp(intd(rs(x, lo), rs(x, hi))(mt), min, max);
    };

    edges_t g;
    int V, X = 0, Y = 0, Z = 0, n = 0, k = 0, levels = 0, min_width = 0;
    double p = 0.0;

    switch (i) {
    case FN_UNIFORM_SPARSE:
        V = dV(S, 0.88), p = reald(2.5, 3.5)(mt) / V;
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_MODERATE:
        V = dV(S, 0.75), p = reald(5.0, 6.0)(mt) / sqrt(V);
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_DENSE:
        V = dV(S, 0.71), p = reald(0.50, 0.95)(mt);
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_LEVEL_LONG_SPARSE:
        V = dV(S, 0.82), p = reald(8.0, 10.0)(mt) / V;
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = dV(S, 0.74), p = reald(8.0, 10.0)(mt) / V;
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_UNIFORM_LEVEL_LONG_MODERATE:
        V = dV(S, 0.82), p = reald(5.0, 6.0)(mt) / sqrt(V);
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = dV(S, 0.74), p = reald(5.0, 6.0)(mt) / sqrt(V);
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_UNIFORM_LEVEL_LONG_DENSE:
        V = dV(S, 0.82), p = reald(0.7, 0.85)(mt);
        levels = roughly(V, 0.55, 0.70, 3, V);
        min_width = roughly(V, 0.15, 0.30, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_UNIFORM_LEVEL_WIDE_DENSE:
        V = dV(S, 0.74), p = reald(0.7, 0.85)(mt);
        levels = roughly(V, 0.20, 0.35, 3, V);
        min_width = roughly(V, 0.45, 0.60, 1, (V - 2) / (levels - 2));
        g = random_uniform_level_flow(V, p, levels, min_width);
        break;
    case FN_COMPLETE:
        V = dV(S, 0.72);
        g = complete_directed(V);
        break;
    case FN_NORMAL_GRID2:
        X = dV(S, 0.455), Y = dV(S, 0.455), V = X * Y;
        g = grid_directed(X, Y);
        break;
    case FN_CIRCULAR_GRID2:
        X = dV(S, 0.450), Y = dV(S, 0.450), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case FN_NORMAL_GRID3:
        X = dV(S, 0.290), Y = dV(S, 0.290), Z = dV(S, 0.290), V = X * Y * Z;
        g = grid3_directed(X, Y, Z);
        break;
    case FN_CIRCULAR_GRID3:
        X = dV(S, 0.288), Y = dV(S, 0.288), Z = dV(S, 0.288), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case FN_KCONNECTED_COMPLETE:
        n = dV(S, 0.57), k = dV(S, 0.28), V = n * k;
        g = k_connected_complete_directed(n, k);
        break;
    default:
        throw runtime_error("Invalid flow network kind");
    }
    assert(verify_edges_directed(g, V) && is_rooted_directed(g, V));

    int E = g.size();
    flow_network fn;
    fn.g = move(g);
    fn.kind = i;
    fn.V = V, fn.E = E, fn.S = S, fn.s = 0, fn.t = V - 1;
    fn.X = X, fn.Y = Y, fn.Z = Z, fn.levels = levels, fn.min_width = min_width;
    fn.n = n, fn.k = k, fn.p = p;
    return fn;
}

auto add_cap_flow_network(flow_network& fn, flow_t mincap, flow_t maxcap) {
    fn.cap = int_gen<flow_t>(fn.E, mincap, maxcap);
}
auto add_cap_flow_network(flow_network& fn, flow_t maxcap) {
    return add_cap_flow_network(fn, 1, maxcap);
}
auto add_cost_flow_network(flow_network& fn, cost_t mincost, cost_t maxcost) {
    fn.cost = int_gen<cost_t>(fn.E, mincost, maxcost);
}
auto add_cost_flow_network(flow_network& fn, cost_t maxcost) {
    return add_cost_flow_network(fn, 1, maxcost);
}

#endif // GEN_FLOW_HPP
