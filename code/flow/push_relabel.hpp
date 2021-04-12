#ifndef PUSH_RELABEL_HPP
#define PUSH_RELABEL_HPP

#include "../struct/integer_lists.hpp" // linked_lists

// *****

using edges_t = vector<array<int, 2>>;

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

#endif // PUSH_RELABEL_HPP
