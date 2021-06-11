#pragma once

#include "push_relabel.hpp"

/**
 * Cost scaling push relabel for general mincost single-commodity flow
 * Complexity: O(V^2 E log(VC))
 *
 * Currently no heuristics are implemented, and only lightly tested.
 *
 * Uses push_relabel from maximum_flow.hpp for feasibility checking.
 * The initial maxflow computation is not required.
 * The costs should be such that (V+1)C can be represented by Cost.
 */
template <typename Flow = long, typename Cost = long, //
          typename FlowSum = Flow, typename CostSum = Cost>
struct mincost_push_relabel {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
        Cost cost;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;
    vector<FlowSum> supply;

    explicit mincost_push_relabel(int V = 0) : V(V), res(V), supply(V) {}
    mincost_push_relabel(int V, vector<FlowSum> sup) : V(V), res(V), supply(move(sup)) {}

    void add(int u, int v, Flow capacity, Cost cost) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0 && cost >= 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0, cost});
        res[v].push_back(E++), edge.push_back({{v, u}, 0, 0, -cost});
    }

    bool balanced() const {
        auto total = accumulate(begin(supply), end(supply), FlowSum(0));
        return total == 0;
    }

    /**
     * Reduce to unweighed max flow to verify feasibility.
     * The core algorithm asserts feasibility for simplicity,
     * so call this first if in doubt. Otherwise just delete it.
     */
    bool feasible() {
        FlowSum total_supply = 0, total_demand = 0;
        for (int u = 0; u < V; u++) {
            if (supply[u] > 0) {
                total_supply += supply[u];
            } else if (supply[u] < 0) {
                total_demand += -supply[u];
            }
        }
        if (total_supply != total_demand) {
            return false;
        }

        push_relabel<Flow, FlowSum> mf(V + 2);
        int s = V, t = V + 1;

        for (int e = 0; e < E; e += 2) {
            auto [u, v] = edge[e].node;
            mf.add(u, v, edge[e].cap);
        }
        for (int u = 0; u < V; u++) {
            if (supply[u] > 0) {
                mf.add(s, u, supply[u]);
            } else if (supply[u] < 0) {
                mf.add(u, t, -supply[u]);
            }
        }

        auto maxflow = mf.maxflow(s, t);
        return maxflow == total_supply;
    }

    vector<CostSum> pi;
    vector<FlowSum> excess;
    vector<int> arc;
    queue<int> active;
    Cost epsilon = 0; // scaling
    static inline constexpr CostSum costsumninf = numeric_limits<CostSum>::min() / 3;

    CostSum reduced_cost(int e) const {
        auto [u, v] = edge[e].node;
        return edge[e].cost + pi[u] - pi[v];
    }
    bool admissible(int e) const {
        return edge[e].flow < edge[e].cap && reduced_cost(e) < 0;
    }
    void scale() {
        for (int e = 0; e < E; e++) {
            edge[e].cost *= V + 1;
        }
    }
    void unscale() {
        for (int e = 0; e < E; e++) {
            edge[e].cost /= V + 1;
        }
    }

    void push(int e) {
        push(e, min(excess[edge[e].node[0]], edge[e].cap - edge[e].flow));
    }
    void push(int e, Flow send) {
        auto [u, v] = edge[e].node;
        assert(send > 0);
        if (excess[v] <= 0 && excess[v] + send > 0) {
            active.push(v);
        }
        edge[e].flow += send;
        edge[e ^ 1].flow -= send;
        excess[u] -= send;
        excess[v] += send;
    }

    void relabel(int u) {
        auto good = pi[u] - epsilon;
        auto pmax = costsumninf;
        int vsize = res[u].size();
        for (int i = 0; i < vsize; i++) {
            int e = res[u][i], v = edge[e].node[1];
            if (edge[e].flow < edge[e].cap) {
                if (pmax < pi[v] - edge[e].cost) {
                    pmax = pi[v] - edge[e].cost;
                    if (good < pmax) {
                        pi[u] = good;
                        arc[u] = i;
                        return;
                    }
                }
            }
        }
        assert(pmax > costsumninf); // oops, infeasible!
        pi[u] = pmax - epsilon;
        arc[u] = 0;
    }

    void discharge(int u) {
        int vsize = res[u].size();
        int& i = arc[u];
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel(u);
            }
            int e = res[u][i];
            if (admissible(e)) {
                push(e);
            }
            i += excess[u] > 0;
        }
    }

    void init_excess() {
        excess = supply;
        for (int u = 0; u < V; u++) {
            if (excess[u] > 0) {
                active.push(u);
            }
        }
    }

    void refine() {
        // saturate admissible edges and init active nodes
        for (int e = 0; e < E; e++) {
            if (admissible(e)) {
                push(e, edge[e].cap - edge[e].flow);
            }
        }

        // there are no admissible arcs, every active node must be relabeled
        for (int u = 0; u < V; u++) {
            arc[u] = res[u].size();
        }

        while (!active.empty()) {
            int u = active.front();
            active.pop();
            discharge(u);
        }
    }

    void optimize() {
        constexpr long alpha = 5;
        Cost C = 0;
        for (int e = 0; e < E; e++) {
            C = max(C, edge[e].cost);
        }
        epsilon = (V + 1) * C;
        do {
            epsilon = max(epsilon / alpha, 1L);
            refine();
        } while (epsilon != 1);
    }

    CostSum mincost_circulation() {
        pi.assign(V, 0);
        arc.assign(V, 0);
        init_excess();
        scale();
        optimize();
        unscale();

        CostSum total_cost = 0;
        for (int e = 0; e < E; e += 2) {
            total_cost += FlowSum(edge[e].flow) * CostSum(edge[e].cost);
        }
        return total_cost;
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
};
