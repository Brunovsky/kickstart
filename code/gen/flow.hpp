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

struct flow_network {
    edges_t g;
    flow_network_kind kind;
    int V, E, S, s, t;
    vector<long> cap;  // edge capacities
    vector<long> cost; // edge costs
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

    auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    auto dense = [&]() { return densed(mt); };
    auto longlevel = [&]() {
        levels = min(V, intd(20, max(20, V / 5))(mt)), m = min(8, V / levels);
    };
    auto widelevel = [&]() {
        levels = min(V, intd(5, clamp(V, 5, 12))(mt)), m = min(20, V / levels);
    };

    switch (i) {
    case FN_UNIFORM_SPARSE:
        V = dV(1.00), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_MODERATE:
        V = dV(0.75), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_DENSE:
        V = dV(0.68), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case FN_UNIFORM_LEVEL_LONG_SPARSE:
        V = dV(0.80), p = sparse(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_SPARSE:
        V = dV(0.75), p = sparse(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_UNIFORM_LEVEL_LONG_MODERATE:
        V = dV(0.80), p = moderate(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_MODERATE:
        V = dV(0.75), p = moderate(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_UNIFORM_LEVEL_LONG_DENSE:
        V = dV(0.90), p = dense(), longlevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_UNIFORM_LEVEL_WIDE_DENSE:
        V = dV(0.75), p = dense(), widelevel();
        g = random_uniform_level_flow(V, p, levels, m);
        break;
    case FN_COMPLETE:
        V = dV(0.68);
        g = complete_directed(V);
        break;
    case FN_NORMAL_GRID2:
        X = dV(0.57), Y = dV(0.57), V = X * Y;
        g = grid_directed(X, Y);
        break;
    case FN_CIRCULAR_GRID2:
        X = dV(0.57), Y = dV(0.57), V = X * Y;
        g = circular_grid_directed(X, Y);
        break;
    case FN_NORMAL_GRID3:
        X = dV(0.37), Y = dV(0.37), Z = dV(0.37), V = X * Y * Z;
        g = grid3_directed(X, Y, Z);
        break;
    case FN_CIRCULAR_GRID3:
        X = dV(0.37), Y = dV(0.37), Z = dV(0.37), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z);
        break;
    case FN_KCONNECTED_COMPLETE:
        m = dV(0.62), k = dV(0.30), V = m * k;
        g = k_connected_complete_directed(m, k);
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
