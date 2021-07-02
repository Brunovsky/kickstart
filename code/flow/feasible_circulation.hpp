#pragma once

#include "push_relabel.hpp" // or another one

template <typename Flow, typename FlowSum = int64_t>
optional<vector<Flow>> feasible_circulation(const vector<array<int, 2>>& g,
                                            const vector<Flow>& lower,
                                            const vector<Flow>& upper,
                                            vector<Flow> supply) {
    using MaxFlowSolver = push_relabel<Flow, FlowSum>;

    int V = supply.size(), E = g.size();
    assert(g.size() == lower.size() && g.size() == upper.size());

    FlowSum total_supply = 0, total_demand = 0;
    for (int u = 0; u < V; u++) {
        if (supply[u] >= 0) {
            total_supply += supply[u];
        } else {
            total_demand += -supply[u];
        }
    }
    if (total_supply != total_demand) {
        return std::nullopt;
    }

    for (int e = 0; e < E; e++) {
        if (lower[e] > upper[e]) {
            return std::nullopt;
        }
    }

    MaxFlowSolver mf(V + 2);
    int s = V, t = V + 1;

    for (int e = 0; e < E; e++) {
        auto [u, v] = g[e];
        assert(0 <= u && u < V && 0 <= v && v < V);
        supply[u] -= lower[e];
        supply[v] += lower[e];
        mf.add(u, v, upper[e] - lower[e]);
    }
    for (int u = 0; u < V; u++) {
        if (supply[u] > 0) {
            mf.add(s, u, supply[u]);
        } else if (supply[u] < 0) {
            mf.add(u, t, -supply[u]);
        }
    }

    auto max_flow = mf.maxflow(s, t);
    if (max_flow != total_supply) {
        return std::nullopt;
    }

    vector<Flow> flow(E);
    for (int e = 0; e < E; e++) {
        flow[e] = lower[e] + mf.get_flow(e);
    }
    return flow;
}

template <typename Flow, typename FlowSum = int64_t>
optional<vector<Flow>> feasible_circulation(const vector<array<int, 2>>& g,
                                            const vector<Flow>& cap,
                                            vector<Flow> supply) {
    return feasible_circulation(g, vector<Flow>(g.size(), 0), cap, supply);
}
