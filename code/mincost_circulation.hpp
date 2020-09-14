#ifndef MINCOST_CIRCULATION_HPP
#define MINCOST_CIRCULATION_HPP

#include "maximum_flow.hpp"

// *****

/**
 * Cost scaling push relabel for general mincost single-commodity flow
 * Complexity: O(V^2 E log(VC))
 *
 * Currently no heuristics are implemented, and only lightly tested.
 *
 * Uses push_relabel from maximum_flow.hpp for feasibility checking.
 * The initial maxflow computation is not required.
 */
struct mincost_push_relabel {
    int V, E = 0;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> supply, flow, cap, cost;

    explicit mincost_push_relabel(int V = 0) : V(V), adj(V), rev(V), res(V), supply(V) {}

    int other(int e, int u) const { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0 && w >= 0);
        assert(w < inf / (V + 1));
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        cap.push_back(c), cap.push_back(0);
        cost.push_back(w), cost.push_back(-w);
        E++;
    }

    bool balanced() const { return accumulate(begin(supply), end(supply), 0L) == 0; }

    /**
     * Reduce to unweighed max flow to verify feasibility.
     * The core algorithm asserts feasibility for simplicity,
     * so call this first if in doubt.
     */
    bool feasible() {
        long total_supply = 0, total_demand = 0;
        for (int u = 0; u < V; u++)
            if (supply[u] > 0)
                total_supply += supply[u];
            else if (supply[u] < 0)
                total_demand += -supply[u];

        if (total_supply != total_demand)
            return false;

        push_relabel mf(V + 2);
        int s = V, t = V + 1;
        for (int e = 0; e < 2 * E; e += 2)
            mf.add(source[e], target[e], cap[e]);
        for (int u = 0; u < V; u++)
            if (supply[u] > 0)
                mf.add(s, u, supply[u]);
            else if (supply[u] < 0)
                mf.add(u, t, -supply[u]);

        long maxflow = mf.maxflow(s, t);
        return maxflow == total_supply;
    }

    vector<long> pi, excess;
    vector<int> arc;
    queue<int> active;
    long epsilon = 0;
    static inline constexpr long inf = LONG_MAX / 3;

    long reduced_cost(int e) const { return cost[e] + pi[source[e]] - pi[target[e]]; }
    bool admissible(int e) const { return flow[e] < cap[e] && reduced_cost(e) < 0; }

    void scale() {
        for (int e = 0; e < 2 * E; e++)
            cost[e] *= V + 1;
    }

    void unscale() {
        for (int e = 0; e < 2 * E; e++)
            cost[e] /= V + 1;
    }

    void push(int e) { push(e, min(excess[source[e]], cap[e] - flow[e])); }
    void push(int e, long send) {
        int u = source[e], v = target[e];
        assert(send > 0);
        if (excess[v] <= 0 && excess[v] + send > 0) {
            active.push(v);
        }
        flow[e] += send;
        flow[e ^ 1] -= send;
        excess[u] -= send;
        excess[v] += send;
    }

    void relabel(int u) {
        long good = pi[u] - epsilon;
        long pmax = LONG_MIN;
        int vsize = res[u].size();
        for (int i = 0; i < vsize; i++) {
            int e = res[u][i], v = target[e];
            if (flow[e] < cap[e]) {
                if (pmax < pi[v] - cost[e]) {
                    pmax = pi[v] - cost[e];
                    if (good < pmax) {
                        pi[u] = good;
                        arc[u] = i;
                        return;
                    }
                }
            }
        }
        assert(pmax > LONG_MIN); // oops, infeasible!
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
        for (int u = 0; u < V; u++)
            if (excess[u] > 0)
                active.push(u);
    }

    void refine() {
        // saturate admissible edges and init active nodes
        for (int e = 0; e < 2 * E; e++)
            if (admissible(e))
                push(e, cap[e] - flow[e]);

        // there are no admissible arcs, every active node must be relabeled
        for (int u = 0; u < V; u++)
            arc[u] = res[u].size();

        while (!active.empty()) {
            int u = active.front();
            active.pop();
            discharge(u);
        }
    }

    void optimize() {
        constexpr long alpha = 5;
        long C = *max_element(begin(cost), end(cost));
        epsilon = (V + 1) * C;
        do {
            epsilon = max(epsilon / alpha, 1L);
            refine();
        } while (epsilon != 1);
    }

    long mincost_circulation() {
        pi.assign(V, 0);
        arc.assign(V, 0);
        flow.assign(2 * E, 0);
        init_excess();
        scale();
        optimize();
        unscale();

        long total_cost = 0;
        for (int e = 0; e < 2 * E; e += 2)
            total_cost += flow[e] * cost[e];
        return total_cost;
    }
};

#endif // MINCOST_CIRCULATION_HPP