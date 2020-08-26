#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr long inf = LONG_MAX / 2;

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(VE^2), not good
 */
struct edmonds_karp {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit edmonds_karp(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    vector<int> pred;

    bool bfs(int s, int t) {
        fill(begin(pred), end(pred), -1);
        vector<int> bfs{s};
        int i = 0, S = 1;
        while (i < S && pred[t] == -1) {
            int u = bfs[i++];
            for (int e : res[u]) {
                int v = target[e];
                if (pred[v] == -1 && v != s && flow[e] < cap[e]) {
                    pred[v] = e;
                    bfs.push_back(v), S++;
                }
            }
        }
        return pred[t] != -1;
    }

    long maxflow(int s, int t) {
        flow.assign(E, 0);
        pred.resize(V);
        long max_flow = 0;
        while (bfs(s, t)) {
            long aug_flow = inf;
            for (int e = pred[t]; e != -1; e = pred[source[e]]) {
                aug_flow = min(aug_flow, cap[e] - flow[e]);
            }
            for (int e = pred[t]; e != -1; e = pred[source[e]]) {
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
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit dinitz_flow(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    // normal nodes are 1..V
    void add(int u, int v, long c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    vector<int> level, arc;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        level[s] = 0;
        vector<int> bfs{s};
        int i = 0, S = 1;
        while (i < S) {
            int u = bfs[i++];
            for (int e : res[u]) {
                int v = target[e];
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
        for (int &i = arc[u], end = res[u].size(); i < end; i++) {
            int e = res[u][i], v = target[e];
            if (flow[e] < cap[e] && level[u] < level[v]) {
                long df = dfs(v, t, min(mincap - preflow, cap[e] - flow[e]));
                flow[e] += df;
                flow[e ^ 1] -= df;
                preflow += df;
                if (preflow == mincap)
                    break;
            }
        }
        return preflow;
    }

    long maxflow(int s, int t) {
        flow.assign(E, 0);
        level.assign(V + 2, 0);
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
 * Push relabel with highest label selection rule, gap heuristic and reverse bfs
 * Also known as Preflow-Push (GAP)
 * Complexity: O(V^2 E^1/2)
 */
struct push_relabel {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit push_relabel(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    // normal nodes are 1..V
    void add(int u, int v, long c) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    vector<long> excess;
    vector<int> height, arc, labeled;
    vector<vector<int>> active;
    int b;

    void push(int e) {
        int u = source[e], v = target[e];
        long send = min(excess[u], cap[e] - flow[e]);
        assert(send > 0);
        if (excess[v] == 0) {
            active[height[v]].push_back(v);
        }
        flow[e] += send;
        flow[e ^ 1] -= send;
        excess[u] -= send;
        excess[v] += send;
    }

    void relabel(int u) {
        labeled[height[u]]--;
        height[u] = 2 * V;
        int vsize = res[u].size();
        for (int i = 0; i < vsize; i++) {
            int e = res[u][i], v = target[e];
            if (flow[e] < cap[e] && height[u] > height[v] + 1) {
                height[u] = height[v] + 1;
                arc[u] = i;
            }
        }
        labeled[height[u]]++;
        if (b < V && !labeled[b]) {
            for (int v = 0; v < V; v++) {
                if (b < height[v] && height[v] < V) {
                    labeled[height[v]]--;
                    height[v] = V + 1;
                }
            }
        }
        b = height[u];
    }

    void discharge(int u) {
        int vsize = res[u].size();
        int& i = arc[u];
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel(u);
            }
            int e = res[u][i];
            int v = target[e];
            if (flow[e] < cap[e] && height[u] > height[v]) {
                push(e);
            }
            i += excess[u] > 0;
        }
    }

    void bfs(int s, int t) {
        vector<int> bfs{t};
        int i = 0, S = 1;
        while (i++ < S) {
            int v = bfs[i - 1];
            for (int e : rev[v]) {
                int u = source[e];
                if (u != t && height[u] == 0) {
                    height[u] = height[v] + 1;
                    bfs.push_back(u), S++;
                } else if (u != s) {
                    height[u] = min(height[u], height[v] + 1);
                }
            }
        }
        for (int u = 0; u < V; u++) {
            labeled[height[u]]++;
        }
    }

    long maxflow(int s, int t) {
        excess.assign(V, 0);
        height.assign(V, 0);
        arc.assign(V, 0);
        flow.assign(E, 0);
        active.assign(2 * V, {});
        labeled.assign(2 * V, 0);
        height[s] = V, height[t] = 0;

        bfs(s, t);
        b = V - 1;

        excess[s] = inf;
        for (int e : adj[s]) {
            push(e);
        }

        while (true) {
            while (b >= 0 && active[b].empty())
                b--;
            if (b < 0)
                break;
            int u = active[b].back();
            active[b].pop_back();
            if (u != s && u != t)
                discharge(u);
        }

        return inf - excess[s];
    }

    bool left_of_mincut(int u) const { return height[u] >= V; }
};

/**
 * Simple, exponential complexity max flow algorithm
 * Complexity: O(2^E f) where f is the maximum flow
 */
struct naive_flow {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit naive_flow(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    vector<bool> vis;

    long dfs(int u, int t, long mincap) {
        if (u == t) {
            return mincap;
        }
        long preflow = 0;
        vis[u] = true;
        vector<int> seen;

        for (int e : res[u]) {
            int v = target[e];

            if (!vis[v] && flow[e] < cap[e]) {
                long pathflow = dfs(v, t, min(mincap - preflow, cap[e] - flow[e]));
                if (pathflow == 0) {
                    vis[v] = true;
                    seen.push_back(v);
                } else {
                    flow[e] += pathflow;
                    flow[e ^ 1] -= pathflow;
                    preflow += pathflow;
                    if (preflow == mincap)
                        break;
                }
            }
        }

        for (int v : seen) {
            vis[v] = false;
        }

        vis[u] = false;
        return preflow;
    }

    long maxflow(int s, int t) {
        vis.assign(V, false);
        return dfs(s, t, LONG_MAX / 2);
    }
};

/**
 * Simple tidal flow algorithm
 * Complexity: O(V E^2)
 * Based on "Tidal Flow: A Fast and Teachable Maximum Flow Algorithm" by Fontaine, M.C.
 * Slower than push-relabel, comparable to dinitz
 */
struct tidal_flow {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap;

    explicit tidal_flow(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv);
        res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    vector<int> level, edges;
    vector<long> p, h, l;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), -1);
        edges.clear();
        level[s] = 0;
        vector<int> bfs{s};
        int i = 0, S = 1;
        while (i < S) {
            int u = bfs[i++];
            for (int e : res[u]) {
                int v = target[e];
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
            int w = source[e], v = target[e];
            p[e] = min(cap[e] - flow[e], h[w]);
            h[v] = h[v] + p[e];
        }
        if (h[t] == 0) {
            return 0;
        }
        fill(begin(l), end(l), 0);
        l[t] = h[t];
        for (auto it = edges.rbegin(); it != edges.rend(); it++) {
            int e = *it, w = source[e], v = target[e];
            p[e] = min(p[e], min(h[w] - l[w], l[v]));
            l[v] -= p[e];
            l[w] += p[e];
        }
        fill(begin(h), end(h), 0);
        h[s] = l[s];
        for (auto e : edges) {
            int w = source[e], v = target[e];
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
};
