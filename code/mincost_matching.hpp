#ifndef MINCOST_MATCHING_HPP
#define MINCOST_MATCHING_HPP

#include "debug_print.hpp"
#include "hash.hpp"

// *****

constexpr long inf = LONG_MAX / 3;
constexpr int inf_dist = INT_MAX;

/**
 * Min-cost perfect bipartite matching (hungarian, lazy repricing)
 * Complexity: O(V^2 E)
 * Based on http://timroughgarden.org/w16/l/l5.pdf
 * nil is U
 */
struct mincost_perfect_matching {
    int V, E, nil;
    vector<vector<int>> adj, rev;
    unordered_map<pair<int, int>, long, pair_hasher> cost;

    explicit mincost_perfect_matching(int V = 0) : V(V), E(0), nil(V), adj(V), rev(V) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && w >= 0);
        assert(!cost.count({u, v}));
        adj[u].push_back(v);
        rev[v].push_back(u);
        cost[{u, v}] = w;
        E++;
    }

    vector<int> mu, mv;  // mu[u]=v, mv[v] = u, where the mapping is u<->v
    vector<long> pu, pv; // pu[u]: price of u, pv[v]: price of v
    vector<int> dist;    // dist[u]: distance of u to s
    vector<int> vis;     // vis[u]: last iteration that u was visited
    int round;

    bool bfs() {
        queue<int> Q;
        for (int u = 0; u < V; u++) {
            if (mu[u] == nil) {
                dist[u] = 0;
                Q.push(u);
            } else {
                dist[u] = inf_dist;
            }
        }
        dist[nil] = inf_dist;
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            if (dist[u] < dist[nil]) {
                for (int v : adj[u]) {
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
                    if (cost[{u, v}] == pu[u] + pv[v] && dist[mv[v]] == inf_dist) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[nil] != inf_dist;
    }

    void reprice() {
        assert(dist[nil] == inf_dist);
        long delta = inf;
        for (int u = 0; u < V; u++) {
            if (dist[u] < inf_dist) {
                for (int v : adj[u]) {
                    if (cost[{u, v}] > pu[u] + pv[v]) {
                        delta = min(delta, cost[{u, v}] - pu[u] - pv[v]);
                    }
                }
            }
        }
        assert(0 < delta && delta < inf); // not trivial
        for (int u = 0; u < V; u++) {
            if (dist[u] < inf_dist) {
                pu[u] += delta;
            }
        }
        for (int v = 0; v < V; v++) {
            if (dist[mv[v]] < inf_dist) {
                pv[v] -= delta;
            }
        }
    }

    bool dfs(int u) {
        if (u == nil) {
            return true;
        }
        if (vis[u] == round) {
            return false;
        }
        vis[u] = round;
        for (int v : adj[u]) {
            if (cost[{u, v}] == pu[u] + pv[v] && dist[mv[v]] > dist[u] && dfs(mv[v])) {
                mv[v] = u;
                mu[u] = v;
                return true;
            }
        }
        return false;
    }

    long compute() {
        mu.assign(V + 1, nil);
        mv.assign(V + 1, nil);
        pu.assign(V + 1, 0);
        pv.assign(V + 1, 0);
        vis.assign(V + 1, 0);
        dist.assign(V + 1, 0);
        round = 0;
        int matchings = 0;
        while (matchings < V) {
            round++;
            if (bfs()) {
                for (int u = 0; u < V; u++) {
                    if (mu[u] == V && dfs(u)) {
                        matchings++;
                    }
                }
            } else {
                reprice();
            }
        }
        long min_cost = 0;
        for (int u = 0; u < V; u++)
            if (mu[u] != nil)
                min_cost += cost[{u, mu[u]}];
        return min_cost;
    }
};

/**
 * Min-cost maximum bipartite matching (hungarian, dijkstra-based)
 * Complexity: O(E min(U,V) log E)
 * Based on https://web.stanford.edu/class/cs261/min_cost_bipartite_matching.pdf
 */
struct mincost_maximum_matching {
    int U, V, E, nil;
    vector<vector<int>> adj, rev;
    unordered_map<pair<int, int>, long, pair_hasher> cost;

    explicit mincost_maximum_matching(int V = 0) : mincost_maximum_matching(V, V) {}
    mincost_maximum_matching(int U, int V) : U(U), V(V), E(0), nil(U), adj(U), rev(V) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        assert(!cost.count({u, v}));
        adj[u].push_back(v);
        rev[v].push_back(u);
        cost[{u, v}] = w;
        E++;
    }

    vector<int> m[2], prev[2];
    vector<long> pi[2], dist[2];

    long reduced_cost(int u, int v) { return cost.at({u, v}) + pi[0][u] - pi[1][v]; }

    bool dijkstra() {
        dist[0].assign(U + 1, inf);
        dist[1].assign(V, inf);
        prev[0].assign(U + 1, -1);
        prev[1].assign(V, -1);
        dist[0][nil] = inf;

        using int2 = pair<long, int>;
        vector<bool> vis(U + 1, false);
        priority_queue<int2, vector<int2>, greater<int2>> Q;
        for (int u = 0; u < U; u++)
            if (m[0][u] == nil)
                dist[0][u] = 0, Q.push({0, u});

        while (!Q.empty()) {
            int u = Q.top().second;
            Q.pop();
            if (vis[u] || u == nil) {
                continue;
            }
            vis[u] = true;
            for (int v : adj[u]) {
                int y = m[1][v];
                long w = dist[0][u] + reduced_cost(u, v);
                assert(reduced_cost(u, v) >= 0);
                if (!vis[y] && dist[1][v] > w) {
                    dist[0][y] = w, dist[1][v] = w;
                    prev[0][y] = v, prev[1][v] = u;
                    assert(y == nil || reduced_cost(y, v) == 0);
                    Q.push({dist[0][y], y});
                }
            }
        }
        for (int u = 0; u < U; u++) {
            pi[0][u] = min(dist[0][u] + pi[0][u], inf);
        }
        for (int v = 0; v < V; v++) {
            pi[1][v] = min(dist[1][v] + pi[1][v], inf);
        }
        return dist[0][nil] != inf;
    }

    auto make_path() {
        vector<int> path;
        int u = prev[0][nil];
        while (u != -1) {
            path.push_back(u);
            u = prev[1][u];
            path.push_back(u);
            u = prev[0][u];
        }
        return path;
    }

    long compute() {
        m[0].assign(U + 1, nil);
        m[1].assign(V, nil);
        pi[0].assign(U + 1, 0);
        pi[1].assign(V, 0);
        dist[0].assign(U + 1, 0);
        dist[1].assign(V, 0);
        prev[0].assign(U + 1, -1);
        prev[1].assign(V, -1);
        int matchings = 0;
        while (matchings < min(U, V) && dijkstra()) {
            auto path = make_path();
            int n = path.size();
            for (int i = 0; i < n; i += 2) {
                int v = path[i], u = path[i + 1];
                m[0][u] = v, m[1][v] = u;
            }
            matchings++;
        }
        long min_cost = 0;
        for (int u = 0; u < V; u++)
            if (m[0][u] != nil)
                min_cost += cost.at({u, m[0][u]});
        return min_cost;
    }
};

#endif // MINCOST_MATCHING_HPP
