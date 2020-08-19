#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr int nil = -1, inf = INT_MAX;

/**
 * Edmond-Karp maximum flow
 * Complexity: O(VE^2), not good
 */
struct maximum_flow {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<int> cap;
    vector<int> flow;

    maximum_flow(int V) : V(V), E(0) { adj.resize(V, {}); }

    // normal nodes are 1..V
    void add(int u, int v, int capacity) {
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
        int max_flow = 0;
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
            int df = inf;
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
    vector<int> cap;
    vector<int> flow;

    maximum_flow_dinic(int V) : V(V), E(0) { adj.resize(V, {}); }

    // normal nodes are 1..V
    void add(int u, int v, int capacity) {
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
        level.assign(V + 2, nil);
        level[s] = 0;
        queue<int> Q;
        Q.push(s);
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            for (int e : adj[u]) {
                int v = target[e];
                if (level[v] == nil && cap[e] > flow[e]) {
                    level[v] = level[u] + 1;
                    Q.push(v);
                }
            }
        }
        return level[t] != nil;
    }

    int dfs(int u, int t, int f) {
        if (u == t) {
            return f;
        }
        for (int &i = arc[u], end = adj[u].size(); i < end; i++) {
            int e = adj[u][i];
            int v = target[e];
            if (cap[e] > flow[e] && level[u] < level[v]) {
                int df = dfs(v, t, min(f, cap[e] - flow[e]));
                if (df > 0) {
                    flow[e] += df;
                    flow[e ^ 1] -= df;
                    return df;
                }
            }
        }
        return 0;
    }

    int compute(int s, int t) {
        flow.assign(E, 0);
        int max_flow = 0;
        while (bfs(s, t)) {
            arc.assign(V, 0);
            int dfs_flow = 0;
            while ((dfs_flow = dfs(s, t, inf))) {
                max_flow += dfs_flow;
            }
        }
        return max_flow;
    }
};

/**
 * General push relabel (with fifo selection rule)
 * Complexity: O(V^3)
 * Computes graph partitioning directly as a byproduct, and mincut easily follows
 */
struct maximum_flow_push_relabel {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<int> cap;
    vector<int> flow;
    vector<int> mincut;

    maximum_flow_push_relabel(int V) : V(V), E(0) { adj.resize(V, {}); }

    // normal nodes are 1..V
    void add(int u, int v, int capacity) {
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

    vector<int> excess;
    vector<int> height;
    vector<int> arc;
    queue<int> active; // fifo selection, O(V^3)

    void push(int e) {
        int u = source[e], v = target[e];
        int send = min(excess[u], cap[e] - flow[e]);
        flow[e] += send;
        flow[e ^ 1] -= send;
        excess[u] -= send;
        excess[v] += send;
        if (excess[v] == send) {
            active.push(v);
        }
    }

    void relabel(int u) {
        int min_height = inf;
        for (int e : adj[u]) {
            if (cap[e] > flow[e]) {
                min_height = min(min_height, height[target[e]]);
                height[u] = min_height + 1;
            }
        }
    }

    void discharge(int u) {
        int vsize = adj[u].size();
        int& i = arc[u];
        while (excess[u] > 0) {
            if (i == vsize) {
                relabel(u);
                i = 0;
            }
            int e = adj[u][i];
            int v = target[e];
            if (cap[e] > flow[e] && height[u] > height[v]) {
                assert(height[u] == height[v] + 1);
                push(e);
                i += excess[u] > 0;
            } else {
                i++;
            }
        }
    }

    int compute(int s, int t) {
        excess.assign(V, 0);
        height.assign(V, 0);
        arc.assign(V, 0);
        flow.assign(E, 0);

        height[s] = V;
        excess[s] = inf;
        for (int e : adj[s]) {
            push(e);
            active.push(target[e]);
        }

        while (!active.empty()) {
            int u = active.front();
            active.pop();
            if (u != s && u != t) {
                discharge(u);
            }
        }

        int max_flow = 0;
        for (int e : adj[s]) {
            max_flow += flow[e];
        }
        return max_flow;
    }

    bool left_of_mincut(int u) const { return height[u] >= V; }

    void compute_mincut() {
        mincut.clear();
        for (int u = 0; u < V; u++) {
            if (left_of_mincut(u)) {
                for (int e : adj[u]) {
                    int v = target[e];
                    if (cap[e] > 0 && !left_of_mincut(v)) {
                        mincut.push_back(e);
                    }
                }
            }
        }
    }
};
