#ifndef SHORTEST_PATH
#define SHORTEST_PATH

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 */
struct dijkstra {
    int V;
    vector<vector<pair<int, int>>> adj;

    dijkstra(int V) : V(V) { adj.resize(V, {}); }

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back({v, w});
    }

    vector<int> dist;
    vector<int> prev;

    void compute(int s, int t = -1) {
        dist.assign(V, INT_MAX);
        prev.assign(V, -1);
        dist[s] = 0;
        prev[s] = s;

        using int2 = pair<int, int>;
        vector<bool> vis(V, false);
        priority_queue<int2, vector<int2>, greater<int2>> Q;
        Q.push({0, s});

        while (!Q.empty()) {
            int u = Q.top().second;
            Q.pop();
            if (vis[u]) {
                continue;
            } else if (u == t) {
                break;
            }
            vis[u] = true;
            for (auto edge : adj[u]) {
                int v = edge.first, w = edge.second;
                int cost = dist[u] + w;
                if (!vis[v] && cost < dist[v]) {
                    dist[v] = cost;
                    prev[v] = u;
                    Q.push({cost, v});
                }
            }
        }
    }

    auto path(int v) {
        vector<int> path;
        if (prev[v] == -1) {
            return path;
        }
        int t;
        do {
            path.push_back(v);
            t = v;
            v = prev[v];
        } while (t != v);
        reverse(begin(path), end(path));
        return path;
    }
};

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 */
struct astar {
    int V;
    vector<vector<pair<int, int>>> adj;
    using heuristic_t = const function<int(int)>&;

    astar(int V) : V(V) { adj.resize(V, {}); }

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        adj[u].push_back({v, w});
    }

    vector<int> dist;
    vector<int> prev;

    int compute(int s, int t, heuristic_t h) {
        dist.assign(V, INT_MAX);
        prev.assign(V, -1);
        dist[s] = 0;
        prev[s] = s;

        using int2 = pair<int, int>;
        vector<bool> vis(V, false);
        priority_queue<int2, vector<int2>, greater<int2>> Q;
        Q.push({0, s});

        while (!Q.empty()) {
            int u = Q.top().second;
            Q.pop();
            if (vis[u]) {
                continue;
            } else if (u == t) {
                break;
            }
            vis[u] = true;
            for (auto edge : adj[u]) {
                int v = edge.first, w = edge.second;
                int cost = dist[u] + w;
                if (!vis[v] && cost < dist[v]) {
                    dist[v] = cost;
                    prev[v] = u;
                    Q.push({cost + h(v), v});
                }
            }
        }

        return dist[t];
    }

    auto path(int v) {
        vector<int> path;
        if (prev[v] == -1) {
            return path;
        }
        int t;
        do {
            path.push_back(v);
            t = v;
            v = prev[v];
        } while (t != v);
        reverse(begin(path), end(path));
        return path;
    }
};

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 */
struct floyd_warshall {
    int V, E;
    vector<int> source;
    vector<int> target;
    vector<int> weight;

    floyd_warshall(int V) : V(V), E(0) {}

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }

    vector<vector<int>> dist;
    vector<vector<int>> next;

    void compute() {
        dist.assign(V, vector<int>(V, INT_MAX));
        next.assign(V, vector<int>(V, -1));

        for (int e = 0; e < E; e++) {
            int u = source[e], v = target[e];
            dist[u][v] = weight[e];
            next[u][v] = v;
        }
        for (int u = 0; u < V; u++) {
            dist[u][u] = 0;
            next[u][u] = u;
        }

        for (int k = 0; k < V; k++) {
            for (int u = 0; u < V; u++) {
                for (int v = 0; v < V; v++) {
                    if (dist[u][v] > dist[u][k] + dist[k][v]) {
                        dist[u][v] = dist[u][k] + dist[k][v];
                        next[u][v] = next[u][k];
                    }
                }
            }
        }
    }

    auto path(int u, int v) {
        vector<int> path;
        if (next[u][v] == -1) {
            return path;
        }
        while (u != v) {
            path.push_back(u);
            u = next[u][v];
        }
        path.push_back(v);
        return path;
    }
};

/**
 * For directed graphs with negative weights
 */
struct bellman_ford {
    int V, E;
    vector<int> source;
    vector<int> target;
    vector<int> weight;

    bellman_ford(int V) : V(V), E(0) {}

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }

    vector<int> dist;
    vector<int> prev;

    void compute(int s) {
        dist.assign(V, INT_MAX);
        prev.assign(V, -1);
        dist[s] = 0;
        prev[s] = s;

        bool stop = false;
        for (int k = 1; k < V && !stop; k++) {
            stop = true;
            for (int e = 0; e < E; e++) {
                int u = source[e], v = target[e];
                if (dist[u] > dist[v] + weight[e]) {
                    dist[u] = dist[v] + weight[e];
                    prev[v] = u;
                    stop = false;
                }
            }
        }

        for (int e = 0; e < E; e++) {
            int u = source[e], v = target[e];
            assert(dist[u] + weight[e] >= dist[v]);
        }
    }

    auto path(int v) {
        vector<int> path;
        if (prev[v] == -1) {
            return path;
        }
        int t;
        do {
            path.push_back(v);
            t = v;
            v = prev[v];
        } while (t != v);
        reverse(begin(path), end(path));
        return path;
    }
};

#endif // SHORTEST_PATH
