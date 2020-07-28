#include <bits/stdc++.h>

using namespace std;

// *****

// hungarian version, O(EV)
struct maximal_matching {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu; // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv; // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> vis;
    int iteration;
    static constexpr int nil = -1;

    maximal_matching(int U, int V) : U(U), V(V) {
        adj.assign(U, {});
    }

    void add(int u, int v) {
        adj[u].push_back(v);
    }

    bool dfs(int u) {
        vis[u] = iteration;
        for (int v : adj[u]) {
            if (mv[v] == nil) {
                mu[u] = v;
                mv[v] = u;
                return true;
            }
        }
        for (int v : adj[u]) {
            if (vis[mv[v]] != iteration && dfs(mv[v])) {
                mu[u] = v;
                mv[v] = u;
                return true;
            }
        }
        return false;
    }

    int compute() {
        mu.assign(U, nil);
        mv.assign(V, nil);
        vis.assign(U, 0);
        iteration = 0;
        int new_matchings = 0;
        int matchings = 0;
        do {
            iteration++;
            new_matchings = 0;
            for (int u = 0; u < U; u++) {
                if (mu[u] == nil && dfs(u)) {
                    new_matchings++;
                }
            }
            matchings += new_matchings;
        } while (new_matchings);
        return matchings;
    }
};
constexpr int maximal_matching::nil;

// hopcroft karp version, O(E sqrt(V))
struct maximal_matching_big {
    int U, V;
    vector<vector<int>> adj;
    vector<int> mu; // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv; // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> vis;
    vector<int> dist;
    int iteration;
    static constexpr int nil = 0, inf = INT_MAX;

    maximal_matching_big(int U, int V) : U(U), V(V) {
        adj.assign(U + 1, {});
        dist.assign(U + 1, 0);
    }

    void add(int u, int v) {
        adj[u + 1].push_back(v + 1);
    }

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
                for (int v : adj[u]) {
                    if (dist[mv[v]] == inf) {
                        dist[mv[v]] = dist[u] + 1;
                        Q.push(mv[v]);
                    }
                }
            }
        }
        return dist[nil] != inf;
    }

    bool dfs(int u) {
        if (u == nil) {
            return true;
        }
        if (vis[u] == iteration) {
            return false;
        }
        vis[u] = iteration;
        for (int v : adj[u]) {
            if (dist[mv[v]] == dist[u] + 1 && dfs(mv[v])) {
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
        vis.assign(U + 1, 0);
        int matching = 0;
        iteration = 0;
        while (bfs()) {
            iteration++;
            for (int u = 1; u <= U; u++) {
                if (mu[u] == nil && dfs(u)) {
                    matching++;
                }
            }
        }
        return matching;
    }
};
constexpr int maximal_matching_big::nil, maximal_matching_big::inf;
