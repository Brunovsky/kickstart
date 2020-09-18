#ifndef MAXIMUM_FLOW_HPP
#define MAXIMUM_FLOW_HPP

#include "graph.hpp"
#include "integer_data_structures.hpp"

// *****

using cap_t = vector<long>;

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(VE^2), not good
 */
struct edmonds_karp {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    edmonds_karp(int V, const edges_t& g, const cap_t& gcap)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = gcap[c++];
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
                }
            }
        }
        return pred[t] != -1;
    }

    long maxflow(int s, int t) {
        pred.resize(V);
        long max_flow = 0;
        while (bfs(s, t)) {
            long aug_flow = inf;
            for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
                aug_flow = min(aug_flow, cap[e] - flow[e]);
            }
            for (int e = pred[t]; e != -1; e = pred[edge[e][0]]) {
                flow[e] += aug_flow;
                flow[e ^ 1] -= aug_flow;
            }
            max_flow += aug_flow;
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

    dinitz_flow(int V, const edges_t& g, const cap_t& gcap)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = gcap[c++];
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
                }
            }
        }
        return level[t] != -1;
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
 * Push relabel with highest label selection rule, gap and global relabeling heuristics
 * Also known as Preflow-Push (GAP)
 * Complexity: O(V^2 E^1/2)
 */
struct push_relabel {
    int V, E = 0;
    vector<vector<int>> res;
    edges_t edge;
    vector<long> flow, cap;

    push_relabel(int V, const edges_t& g, const cap_t& gcap)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = gcap[c++];
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
            for (int h = at + 1; h < V; h++) {
                FOR_EACH_IN_LINKED_LIST (v, h, labeled) {
                    height[v] = V + 1;
                    if (excess[v] > 0)
                        active.erase(v), active.push_back(V + 1, v);
                }
                labeled.clear(h);
            }
        }
    }

    void global_relabel(int s, int t, bool sink) {
        vector<int> new_height(V, 2 * V), bfs{sink ? t : s};
        new_height[s] = V, new_height[t] = 0;
        reverse_bfs(bfs, new_height);
        for (int u : bfs) {
            if (height[u] != new_height[u]) {
                height[u] = new_height[u];
                if (excess[u] > 0)
                    active.erase(u), active.push_back(height[u], u);
                if (0 < height[u] && height[u] < V)
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

    void relabel(int u) {
        relabel_count++;
        if (height[u] < V)
            labeled.erase(u);
        height[u] = 2 * V;
        for (int i = 0, vsize = res[u].size(); i < vsize; i++) {
            int e = res[u][i], v = edge[e][1];
            if (flow[e] < cap[e] && height[u] > height[v] + 1) {
                height[u] = height[v] + 1;
                arc[u] = i;
            }
        }
        if (height[u] < V) {
            labeled.push_back(height[u], u);
            gap(b);
        }
        b = height[u];
    }

    void discharge(int u, bool sink) {
        int& i = arc[u];
        int vsize = res[u].size();
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel(u);
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
        active.assign(2 * V, V);
        labeled.assign(V, V);

        init_bfs(s, t);
        relabel_count = 0;

        excess[s] = inf;
        for (int e : res[s]) {
            push(e);
        }
        const int threshold = global_relabel_threshold();

        b = V - 1;
        while (true) {
            b = min(b, V - 1);
            while (b >= 0 && active.empty(b))
                b--;
            if (b < 0)
                break;
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel(s, t, 1);
                b = V - 1;
            } else {
                int u = active.tail(b);
                active.pop_back(b);
                if (u != s && u != t)
                    discharge(u, 1);
            }
        }

        if (value_only)
            return excess[t];

        b = 2 * V - 1;
        while (true) {
            while (b >= V && active.empty(b))
                b--;
            if (b < V)
                break;
            if (relabel_count >= threshold) {
                relabel_count = 0;
                global_relabel(s, t, 0);
                b = 2 * V - 1;
            } else {
                int u = active.head(b);
                active.pop_front(b);
                if (u != s && u != t)
                    discharge(u, 0);
            }
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

    tidal_flow(int V, const edges_t& g, const cap_t& gcap)
        : V(V), E(g.size()), res(V), edge(2 * E), flow(2 * E, 0), cap(2 * E) {
        int e = 0, c = 0;
        for (auto [u, v] : g) {
            res[u].push_back(e), edge[e] = {u, v}, cap[e++] = gcap[c++];
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
        while (i < S) {
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
        }
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
        long max_flow = 0;
        while (bfs(s, t)) {
            long df;
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
