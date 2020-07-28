#include <bits/stdc++.h>

using namespace std;

// *****

// based on https://www.youtube.com/watch?v=Wq2tkITYYHE
struct mincost_matching {
    int U, V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<int> cost;

    mincost_matching(int U, int V) : U(U), V(V), E(0) {
        adj.assign(U + 1, {});
    }

    void add(int u, int v, int edge_cost) {
        assert(0 <= u && u < U && 0 <= v && v < V && edge_cost >= 0);
        int e = E++;
        source.resize(E), target.resize(E), cost.resize(E);
        source[e] = u + 1;
        target[e] = v + 1;
        cost[e] = edge_cost;
        adj[u + 1].push_back(e);
    }

    vector<int> mu;   // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv;   // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> pu;   // pu[u]: price of u
    vector<int> pv;   // pv[v]: price of v
    vector<int> dist; // dist[u]: distance of u to s
    vector<int> vis;  // vis[u]: last iteration that u was visited
    int iteration;
    static constexpr int nil = 0, inf = INT_MAX;

    bool bfs() {
        queue<int> Q;
        for (int u = 1; u <= U; u++) {
            if (mu[u] == nil) {
                dist[u] = 0;
                Q.push(u);
            } else {
                dist[u] = inf;
            }
        }
        dist[nil] = inf;
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            if (dist[u] < dist[nil]) {
                for (int e : adj[u]) {
                    int v = target[e];
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (cost[e] == pu[u] + pv[v] && dist[mv[v]] == inf) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[nil] != inf;
    }

    void reprice() {
        assert(dist[nil] == inf);
        int delta = inf;
        for (int u = 1; u <= U; u++) {
            if (dist[u] < inf) {
                for (int e : adj[u]) {
                    int v = target[e];
                    if (cost[e] > pu[u] + pv[v]) {
                        assert(mu[u] != v && mv[v] != u);
                        delta = min(delta, cost[e] - pu[u] - pv[v]);
                    }
                }
            }
        }
        assert(0 < delta && delta < inf); // not trivial
        for (int u = 1; u <= U; u++) {
            if (dist[u] < inf) {
                pu[u] += delta;
            }
        }
        for (int v = 1; v <= V; v++) {
            if (dist[mv[v]] < inf) {
                pv[v] -= delta;
            }
        }
    }

    bool dfs(int u) {
        if (u == nil) {
            return true;
        }
        if (vis[u] == iteration) {
            return false;
        }
        vis[u] = iteration;
        for (int e : adj[u]) {
            int v = target[e];
            if (cost[e] == pu[u] + pv[v] && dist[mv[v]] == dist[u] + 1 && dfs(mv[v])) {
                mv[v] = u;
                mu[u] = v;
                return true;
            }
        }
        return false;
    }

    int compute() {
        mu.assign(U + 1, nil);
        mv.assign(V + 1, nil);
        pu.assign(U + 1, 0);
        pv.assign(V + 1, 0);
        vis.assign(U + 1, 0);
        dist.assign(U + 1, 0);
        iteration = 0;
        int matchings = 0;
        while (matchings < min(U, V)) {
            iteration++;
            if (bfs()) {
                for (int u = 1; u <= U; u++) {
                    if (mu[u] == nil && dfs(u)) {
                        matchings++;
                    }
                }
            } else {
                reprice();
            }
        };
        int min_cost = 0;
        for (int e = 0; e < E; e++) {
            if (mu[source[e]] == target[e]) {
                min_cost += cost[e];
            }
        }
        return min_cost;
    }
};
constexpr int mincost_matching::nil, mincost_matching::inf;
