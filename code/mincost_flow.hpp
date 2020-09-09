#ifndef MINCOST_FLOW_HPP
#define MINCOST_FLOW_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

static constexpr long inf = LONG_MAX / 3;

/**
 * Edmonds-Karp augmenting paths
 * Complexity: O(V E^2 log V)
 */
struct mincost_edmonds_karp {
    int V, E;
    vector<vector<int>> adj, rev, res;
    vector<int> source, target;
    vector<long> flow, cap, cost;

    explicit mincost_edmonds_karp(int V = 0) : V(V), E(0), adj(V), rev(V), res(V) {}

    int other(int e, int u) { return u == target[e] ? source[e] : target[e]; }

    void add(int u, int v, long c, long w) {
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
        cost.push_back(w), cost.push_back(-w);
        E++;
    }

    vector<long> dist, pi;
    vector<int> prev;

    bool dijkstra(int s, int t) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = 0;

        using int2 = pair<long, int>;
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
            for (int e : res[u]) {
                int v = other(e, u);
                long w = dist[u] + pi[u] - pi[v] + cost[e];
                if (!vis[v] && flow[e] < cap[e] && dist[v] > w) {
                    dist[v] = w;
                    prev[v] = e;
                    Q.push({w, v});
                }
            }
        }
        for (int u = 0; u < V; u++) {
            pi[u] = min(dist[u] + pi[u], inf);
        }
        return vis[t];
    }

    auto path(int v) {
        vector<int> path;
        while (prev[v] != -1) {
            path.push_back(prev[v]);
            v = other(prev[v], v);
        }
        return path;
    }

    pair<long, long> mincost_flow(int s, int t, long F) {
        long sflow = 0;
        while (sflow < F && dijkstra(s, t)) {
            auto augmenting_path = path(t);
            long df = F - sflow;
            for (int e : augmenting_path) {
                df = min(df, cap[e] - flow[e]);
            }
            sflow += df;
            for (int e : augmenting_path) {
                flow[e] += df;
                flow[e ^ 1] -= df;
            }
        }
        long scost = 0;
        for (int e = 0; e < 2 * E; e += 2)
            scost += flow[e] * cost[e];
        return {sflow, scost};
    }

    pair<long, long> mincost_maxflow(int s, int t) { return mincost_flow(s, t, inf); }
};

#endif // MINCOST_FLOW_HPP
