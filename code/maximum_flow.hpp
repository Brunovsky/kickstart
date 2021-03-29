#ifndef MAXIMUM_FLOW_HPP
#define MAXIMUM_FLOW_HPP

#include "graph.hpp"
#include "integer_heaps.hpp"
#include "integer_lists.hpp"

// *****

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(VE^2), not good
 */
struct edmonds_karp {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    edmonds_karp(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> pred;
    static constexpr long inf = LONG_MAX / 2;

    bool bfs(int s, int t) {
        fill(begin(pred), end(pred), -1);
        vector<int> bfs{s};
        int j = 0, S = 1;
        while (j < S && pred[t] == -1) {
            int u = bfs[j++];
            for (auto e : res[u]) {
                int v = edge[e][1];
                if (pred[v] == -1 && v != s && flow[e] < cap[e]) {
                    pred[v] = e;
                    bfs.push_back(v), S++;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    long augment(int t) {
        long aug_flow = inf;
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            aug_flow = min(aug_flow, cap[e] - flow[e]);
        }
        for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
            flow[e] += aug_flow;
            flow[e ^ 1] -= aug_flow;
        }
        return aug_flow;
    }

    long maxflow(int s, int t) {
        pred.resize(V);
        long max_flow = 0;
        while (bfs(s, t)) {
            max_flow += augment(t);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return pred[u] >= 0; }
};

/**
 * Dinitz's blocking flows
 * Complexity: O(V^2 E), close to push relabel in practice
 */
struct dinitz_flow {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    dinitz_flow(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> level, arc;
    static constexpr long inf = LONG_MAX / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        level[s] = 0;
        vector<int> bfs{s};
        int j = 0, S = 1;
        while (j < S) {
            int u = bfs[j++];
            for (int e : res[u]) {
                int v = edge[e][1];
                if (level[v] == -1 && flow[e] < cap[e]) {
                    level[v] = level[u] + 1;
                    bfs.push_back(v), S++;
                    if (v == t)
                        return true;
                }
            }
        }
        return false;
    }

    long dfs(int u, int t, long mincap) {
        if (u == t) {
            return mincap;
        }
        long preflow = 0;
        for (int &i = arc[u], vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && level[u] < level[v]) {
                long df = dfs(v, t, min(mincap, cap[e] - flow[e]));
                flow[e] += df;
                flow[e ^ 1] -= df;
                preflow += df, mincap -= df;
                if (mincap == 0)
                    break;
            }
        }
        return preflow;
    }

    long maxflow(int s, int t) {
        level.assign(V, 0);
        arc.assign(V, 0);
        long max_flow = 0;
        while (bfs(s, t)) {
            max_flow += dfs(s, t, inf);
            fill(begin(arc), end(arc), 0);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

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
        while (j < frontier.size() && frontier[j] != t) {
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

/**
 * Push relabel with highest label selection rule, gap and global relabeling heuristics
 * Also known as Preflow-Push (GAP)
 * Complexity: O(V^2 E^1/2)
 */
struct push_relabel {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    push_relabel(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            assert(u < V && v < V);
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> height, arc;
    vector<long> excess;
    linked_lists active, labeled;
    int relabel_count, b; // current bucket (height)
    static constexpr long inf = LONG_MAX / 2;

    int global_relabel_threshold() const {
        return 1 + int(ceil(log2(1.5 * E + 1) / log2(V + 1) * V));
    }

    auto reverse_bfs(vector<int>& bfs, vector<int>& new_height) {
        int j = 0, S = 1;
        while (j < S) {
            int v = bfs[j++];
            for (int e : res[v]) {
                int u = edge[e][1], r = e ^ 1;
                if (flow[r] < cap[r] && new_height[u] == 2 * V) {
                    new_height[u] = new_height[v] + 1;
                    bfs.push_back(u), S++;
                }
            }
        }
    }

    void init_bfs(int s, int t) {
        vector<int> bfs{t};
        height.assign(V, 2 * V);
        height[s] = V, height[t] = 0;
        reverse_bfs(bfs, height);
        for (int u : bfs) {
            if (u != t && height[u] < V) {
                labeled.push_back(height[u], u);
            }
        }
    }

    void gap(int at) {
        if (at < V && labeled.empty(at)) {
            for (int h = at + 1; h < V && !labeled.empty(h); h++) {
                FOR_EACH_IN_LINKED_LIST (v, h, labeled) {
                    height[v] = V + 1;
                    if (excess[v] > 0)
                        active.erase(v), active.push_back(V + 1, v);
                }
                labeled.clear(h);
            }
        }
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void global_relabel(int s, int t) {
        vector<int> new_height(V, 2 * V), bfs{sink ? t : s};
        new_height[s] = V, new_height[t] = 0;
        reverse_bfs(bfs, new_height);
        for (int u : bfs) {
            if (height[u] != new_height[u]) {
                height[u] = new_height[u];
                if (excess[u] > 0)
                    active.erase(u), active.push_back(height[u], u);
                if (sink && 0 < height[u] && height[u] < V)
                    labeled.erase(u), labeled.push_back(height[u], u);
            }
        }
    }

    void push(int e) {
        auto [u, v] = edge[e];
        long df = min(excess[u], cap[e] - flow[e]);
        assert(df > 0);
        if (excess[v] == 0) {
            active.push_back(height[v], v);
        }
        flow[e] += df;
        flow[e ^ 1] -= df;
        excess[u] -= df;
        excess[v] += df;
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void relabel(int u) {
        relabel_count++;
        if (sink)
            labeled.erase(u);
        height[u] = 2 * V;
        for (int i = 0, vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && height[u] > height[v] + 1) {
                height[u] = height[v] + 1;
                arc[u] = i;
            }
        }
        if (sink && height[u] < V) {
            labeled.push_back(height[u], u);
            gap(b);
        }
        b = height[u];
    }

    template <bool sink> // 1=push phase (heights<V), 0=recover phase (heights>V)
    void discharge(int u) {
        int& i = arc[u];
        int vsize = res[u].size();
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel<sink>(u);
                if (sink && height[u] >= V) {
                    active.push_back(height[u], u);
                    return;
                }
            }
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && height[u] > height[v]) {
                push(e);
            }
            i += excess[u] > 0;
        }
    }

    long maxflow(int s, int t, bool value_only = true) {
        excess.assign(V, 0);
        arc.assign(V, 0);
        active.assign(2 * V + 1, V);
        labeled.assign(V, V);

        init_bfs(s, t);
        relabel_count = 0;

        excess[s] = inf;
        for (int e : res[s]) {
            if (cap[e] > 0)
                push(e);
        }
        const int threshold = global_relabel_threshold();

        b = V - 1;
        while (true) {
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel<1>(s, t);
                b = V - 1;
            }
            b = min(b, V - 1);
            while (b > 0 && active.empty(b))
                b--;
            if (b <= 0)
                break;
            int u = active.tail(b);
            active.pop_back(b);
            discharge<1>(u);
        }

        if (value_only)
            return excess[t];

        b = 2 * V - 1;
        while (true) {
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel<0>(s, t);
                b = 2 * V - 1;
            }
            while (b > V && active.empty(b))
                b--;
            if (b <= V)
                break;
            int u = active.head(b);
            active.pop_front(b);
            discharge<0>(u);
        }

        return excess[t];
    }

    bool left_of_mincut(int u) const { return height[u] >= V; }
};

/**
 * Simple tidal flow algorithm
 * Complexity: O(V E^2)
 * Based on "Tidal Flow: A Fast and Teachable Maximum Flow Algorithm" by Fontaine, M.C.
 * Slower than push-relabel, comparable to dinitz
 */
struct tidal_flow {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    tidal_flow(int V, const edges_t& g, const vector<long>& caps)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = caps[c++];
            res[v].push_back(e), edge[e] = {v, u}, cap[e++] = 0;
        }
    }

    vector<int> level, edges;
    vector<long> p, h, l;
    static constexpr long inf = LONG_MAX / 2;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        edges.clear();
        level[s] = 0;
        vector<int> bfs{s};
        int i = 0, S = 1;
        while (i < S && level[bfs[i]] != level[t]) {
            int u = bfs[i++];
            for (int e : res[u]) {
                int v = edge[e][1];
                if (flow[e] < cap[e]) {
                    if (level[v] == -1) {
                        level[v] = level[u] + 1;
                        bfs.push_back(v), S++;
                    }
                    if (level[v] == level[u] + 1) {
                        edges.push_back(e);
                    }
                }
            }
        }
        return level[t] != -1;
    }

    long tide(int s, int t) {
        fill(begin(h), end(h), 0);
        h[s] = inf;
        for (int e : edges) {
            auto [w, v] = edge[e];
            p[e] = min(cap[e] - flow[e], h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it;
            auto [w, v] = edge[e];
            p[e] = min(p[e], min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        } // 1=push phase, 0=recover phase
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            auto [w, v] = edge[e];
            p[e] = min(p[e], h[w]);
            h[w] -= p[e];
            h[v] += p[e];
            flow[e] += p[e];
            flow[e ^ 1] -= p[e];
        }
        return h[t];
    }

    long maxflow(int s, int t) {
        level.assign(V, 0);
        h.assign(V, 0);
        l.assign(V, 0);
        p.assign(2 * E, 0);
        flow.assign(2 * E, 0);
        long max_flow = 0, df;
        while (bfs(s, t)) {
            do {
                df = tide(s, t);
                max_flow += df;
            } while (df > 0);
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return level[u] >= 0; }
};

#endif // MAXIMUM_FLOW_HPP
