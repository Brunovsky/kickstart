#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr int nil = -1;
static constexpr long inf = LONG_MAX / 2;

/**
 * Edmond-Karp maximum flow
 * Complexity: O(VE^2), not good
 */
struct maximum_flow {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<long> cap;
    vector<long> flow;

    maximum_flow(int V) : V(V), E(0) { adj.resize(V, {}); }

    // normal nodes are 1..V
    void add(int u, int v, long capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        int uv = E++;
        int vu = E++;
        source.resize(E), target.resize(E), cap.resize(E);
        source[uv] = target[vu] = u;
        source[vu] = target[uv] = v;
        cap[uv] = capacity;
        cap[vu] = 0;
        adj[u].push_back(uv);
        adj[v].push_back(vu);
    }

    int compute(int s, int t) {
        flow.assign(E, 0);
        long max_flow = 0;
        do {
            vector<int> pred(V + 2, nil);
            queue<int> Q;
            Q.push(s);
            while (!Q.empty()) {
                int u = Q.front();
                Q.pop();
                for (int e : adj[u]) {
                    int v = target[e];
                    if (pred[v] == nil && v != s && cap[e] > flow[e]) {
                        pred[v] = e;
                        Q.push(v);
                    }
                }
            }
            if (pred[t] == nil) {
                break;
            }
            long df = inf;
            for (int e = pred[t]; e != nil; e = pred[source[e]]) {
                df = min(df, cap[e] - flow[e]);
            }
            for (int e = pred[t]; e != nil; e = pred[source[e]]) {
                flow[e] += df;
                flow[e ^ 1] -= df;
            }
            max_flow += df;
        } while (true);
        return max_flow;
    }
};

/**
 * Dinic's blocking flow algorithm for max flow
 * Complexity: O(V^2E)
 */
struct maximum_flow_dinic {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<long> cap;
    vector<long> flow;

    maximum_flow_dinic(int V) : V(V), E(0) { adj.resize(V, {}); }

    // normal nodes are 1..V
    void add(int u, int v, long capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        int uv = E++;
        int vu = E++;
        source.resize(E), target.resize(E), cap.resize(E);
        source[uv] = target[vu] = u;
        source[vu] = target[uv] = v;
        cap[uv] = capacity;
        cap[vu] = 0;
        adj[u].push_back(uv);
        adj[v].push_back(vu);
    }

    vector<int> level;
    vector<int> arc;

    bool bfs(int s, int t) {
        fill(begin(level), end(level), nil);
        level[s] = 0;
        queue<int> Q;
        Q.push(s);
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            for (int e : adj[u]) {
                int v = target[e];
                if (level[v] == nil && flow[e] < cap[e]) {
                    level[v] = level[u] + 1;
                    Q.push(v);
                }
            }
        }
        return level[t] != nil;
    }

    long dfs(int u, int t, long f) {
        if (u == t) {
            return f;
        }
        long preflow = 0;
        for (int &i = arc[u], end = adj[u].size(); i < end; i++) {
            int e = adj[u][i], v = target[e];
            if (flow[e] < cap[e] && level[u] < level[v]) {
                long df = dfs(v, t, min(f - preflow, cap[e] - flow[e]));
                flow[e] += df;
                flow[e ^ 1] -= df;
                preflow += df;
                if (preflow == f)
                    break;
            }
        }
        return preflow;
    }

    int compute(int s, int t) {
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
};

/**
 * Push relabel with highest label selection rule, gap heuristic and reverse bfs
 * Complexity: O(V^2 E^1/2)
 * Constant factor is smaller for large graphs if edges are represented locally.
 */
struct maximum_flow_push_relabel {
    int V, E;
    vector<vector<int>> adj, rev;
    vector<int> source;
    vector<int> target;
    vector<long> cap;
    vector<long> flow;
    vector<int> mincut;

    maximum_flow_push_relabel(int V) : V(V), E(0), adj(V), rev(V) {}

    // normal nodes are 1..V
    void add(int u, int v, long capacity) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0);
        int uv = E++;
        int vu = E++;
        source.resize(E), target.resize(E), cap.resize(E);
        source[uv] = target[vu] = u;
        source[vu] = target[uv] = v;
        cap[uv] = capacity;
        cap[vu] = 0;
        adj[u].push_back(uv);
        adj[v].push_back(vu);
        rev[v].push_back(u);
    }

    vector<long> excess;
    vector<int> height;
    vector<int> arc;
    vector<vector<int>> active;
    vector<int> labeled;
    int b;

    void push(int e) {
        int u = source[e], v = target[e];
        long send = min(excess[u], cap[e] - flow[e]);
        if (flow[e] < cap[e] && excess[v] == 0) {
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
        int vsize = adj[u].size();
        for (int i = 0; i < vsize; i++) {
            int e = adj[u][i], v = target[e];
            if (flow[e] < cap[e] && height[u] > height[v] + 1) {
                height[u] = height[v] + 1;
                // arc[u] = i;
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
        int vsize = adj[u].size();
        int &i = arc[u];
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel(u);
                i = 0;
            }
            int e = adj[u][i];
            int v = target[e];
            if (flow[e] < cap[e] && height[u] > height[v]) {
                // assert(height[u] == height[v] + 1);
                push(e);
            }
            i += excess[u] > 0;
        }
    }

    int compute(int s, int t) {
        excess.assign(V, 0);
        height.assign(V, 0);
        arc.assign(V, 0);
        flow.assign(E, 0);
        active.assign(2 * V, {});
        labeled.assign(2 * V, 0);
        height[s] = V, height[t] = 0;
        b = V - 1;
        assert(s != t);

        vector<int> bfs{t};
        int i = 0, S = 1;
        while (i++ < S) {
            int v = bfs[i - 1];
            for (int u : rev[v]) {
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

    naive_flow(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c = 1) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && c > 0);
        int uv = 2 * E;
        int vu = 2 * E + 1;
        adj[u].push_back(uv);
        rev[v].push_back(uv);
        res[u].push_back(uv), res[v].push_back(vu);
        source.push_back(u), source.push_back(v);
        target.push_back(v), target.push_back(u);
        flow.push_back(0), flow.push_back(0);
        cap.push_back(c), cap.push_back(0);
        E++;
    }

    int compute(int s, int t) {
        vis.assign(V, false);
        return dfs(s, t, LONG_MAX / 2);
    }

    vector<bool> vis;

    int dfs(int u, int t, long mincap) {
        if (u == t) {
            return mincap;
        }
        long preflow = 0;
        vis[u] = true;
        vector<int> seen;

        for (int e : res[u]) {
            int v = target[e];

            if (!vis[v] && flow[e] < cap[e]) {
                int pathflow = dfs(v, t, min(mincap - preflow, cap[e] - flow[e]));
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

        // printf("OUT %d   mincap=%ld  preflow=%ld\n", u, mincap, preflow);
        vis[u] = false;
        return preflow;
    }
};
