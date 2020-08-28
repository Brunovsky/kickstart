#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Simple maximum matching algorithm
 * Complexity: O(VE)
 */
struct maximum_matching {
    int U, V;
    vector<vector<int>> adj;

    maximum_matching(int U, int V) : U(U), V(V) { adj.assign(U, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> mu;  // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv;  // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> vis; // vis[u]: last iteration that u was visited
    int iteration;

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

  private:
    int nil = -1;
};

/**
 * Hopcroft-Karp maximal matching
 * Complexity: O(V^1/2 E)
 */
struct maximal_matching_big {
    int U, V;
    vector<vector<int>> adj;

    maximal_matching_big(int U, int V) : U(U), V(V) { adj.assign(U + 1, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < U && 0 <= v && v < V);
        adj[u + 1].push_back(v + 1);
    }

    vector<int> mu;   // mu[u]=v where the mapping is u<->v, nil if u is unmatched
    vector<int> mv;   // mv[v]=u where the mapping is u<->v, nil if v is unmatched
    vector<int> vis;  // vis[u]: last iteration that u was visited
    vector<int> dist; // dist[u]: distance of u to s
    int iteration;

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
                    // note: the check v != mu[u] is implicit in dist[mv[v]] == inf
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
        dist.assign(U + 1, 0);
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

  private:
    int nil = 0, inf = INT_MAX;
};
