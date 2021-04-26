#ifndef MPM_FLOW_HPP
#define MPM_FLOW_HPP

#include "../struct/integer_heaps.hpp" // pairing_int_heap

// *****

using edges_t = vector<array<int, 2>>;

/**
 * MPM's blocking flows
 * Complexity: O(V^3), slowest in practice
 */
struct mpm_flow {
    int V, E = 0;
    vector<int> res, off;
    edges_t edge;
    vector<long> flow, cap;

    mpm_flow(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(2 * E), off(V + 1, 0), edge(2 * E), flow(2 * E, 0),
          cap(2 * E) {
        for (auto [u, v] : g)
            off[u + 1]++, off[v + 1]++;
        partial_sum(begin(off), end(off), begin(off));
        vector<int> cur = off;
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[cur[u]++] = e, edge[e] = {u, v}, cap[e++] = caps[c++];
            res[cur[v]++] = e, edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<array<int, 2>> arc;
    vector<array<long, 2>> pi;
    vector<int> level, adj[2], frontier;
    vector<long> excess;

    using heap_t = pairing_int_heap<function<bool(int, int)>>;
    heap_t heap;
    static constexpr long inf = LONG_MAX / 2;

    inline long pmin(int u) const { return min(pi[u][0], pi[u][1]); }

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        level[s] = 0;
        frontier = {s};
        int j = 0, S = 1;
        while (j < S) {
            int u = frontier[j++];
            for (int i = off[u]; i < off[u + 1]; i++) {
                int e = res[i], v = edge[e][1];
                if (level[v] == -1 && flow[e] < cap[e]) {
                    level[v] = level[u] + 1;
                    frontier.push_back(v), S++;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    void remove(int u) {
        for (bool c : {0, 1}) {
            while (arc[u][c] >= 0) {
                int e = adj[c][off[u] + arc[u][c]--], v = edge[e][!c];
                if (flow[e] < cap[e] && heap.contains(v))
                    pi[v][!c] -= cap[e] - flow[e], heap.improve(v);
            }
        }
    }

    template <bool c> // 0=forward to t, 1=backward to s
    void discharge(int u) {
        while (excess[u]) {
            int e = adj[c][off[u] + arc[u][c]], v = edge[e][!c];
            assert(arc[u][c] >= 0 && edge[e][c] == u);

            if (flow[e] < cap[e] && heap.contains(v)) {
                long df = min(excess[u], cap[e] - flow[e]);
                if (excess[v] == 0) {
                    frontier.push_back(v);
                }
                flow[e] += df;
                flow[e ^ 1] -= df;
                excess[u] -= df;
                excess[v] += df;
            }
            arc[u][c] -= excess[u] != 0;
        }
    }

    template <bool c> // 0=push to t, 1=pull from s
    void push(int w, int t, long augf) {
        if (w == t)
            return;
        frontier = {};
        excess[w] = augf;
        discharge<c>(w);
        int j = 0;
        while (j < int(frontier.size()) && frontier[j] != t) {
            int u = frontier[j++];
            pi[u][0] -= excess[u];
            pi[u][1] -= excess[u];
            heap.improve(u);
            discharge<c>(u);
        }
        pi[t][!c] -= augf;
        heap.improve(t);
    }

    long run(int s, int t) {
        for (int e = 0; e < 2 * E; e++) {
            auto [u, v] = edge[e];
            if (flow[e] < cap[e] && level[u] != -1 && level[u] < level[v]) {
                pi[u][0] += cap[e] - flow[e];
                pi[v][1] += cap[e] - flow[e];
                adj[0][off[u] + ++arc[u][0]] = e;
                adj[1][off[v] + ++arc[v][1]] = e;
            }
        }
        pi[s][1] = pi[t][0] = inf;
        assert(heap.empty());
        for (int u = 0; u < V; u++) {
            if (level[u] != -1)
                heap.push(u);
        }
        long flow = 0;
        while (!heap.empty()) {
            int u = heap.pop();
            long df = pmin(u);
            if (df > 0) {
                push<0>(u, t, df);
                push<1>(u, s, df);
                flow += df;
            }
            pi[u][0] = pi[u][1] = 0;
            remove(u);
        }
        return flow;
    }

    long maxflow(int s, int t) {
        arc.assign(V, {-1, -1});
        pi.assign(V, {0, 0});
        level.resize(V);
        excess.assign(V, 0);
        adj[0].resize(2 * E);
        adj[1].resize(2 * E);
        heap = heap_t(V, [&](int u, int v) { return pmin(u) < pmin(v); });
        long max_flow = 0;
        while (bfs(s, t)) {
            max_flow += run(s, t);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // MPM_FLOW_HPP
