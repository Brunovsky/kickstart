#ifndef MINCUT_HPP
#define MINCUT_HPP

#include "hash.hpp"

// *****

/**
 * Stoer-Wagner minimum cut, priority-queue based
 * Complexity: O(VE log(V))
 */
struct stoer_wagner {
    int V, E = 0;
    vector<unordered_set<int>> adj;
    unordered_map<pair<int, int>, long, pair_hasher> cost;

    explicit stoer_wagner(int V = 0) : V(V), adj(V) {}

    void add(int u, int v, long w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        auto e = minmax(u, v);
        assert(!cost.count(e));
        adj[u].insert(v);
        adj[v].insert(u);
        cost[e] = w;
        E++;
    }

    using cut_t = vector<int>;
    using cut_range_t = pair<list<int>::iterator, list<int>::iterator>;
    static inline constexpr long inf = LONG_MAX;

    pair<long, cut_t> minimum_cut(int a = 0) {
        long best_cost = inf;
        cut_range_t best_cut;

        vector<list<int>> node(V);
        for (int u = 0; u < V; u++)
            node[u] = {u};

        for (int phase = V - 1; phase > 0; phase--) {
            using int2 = pair<long, int>;
            vector<long> sum(V, 0);
            vector<bool> vis(V, false);
            priority_queue<int2> Q;
            Q.push({0, a});

            int s, t = a;

            while (!Q.empty()) {
                int u = Q.top().second;
                Q.pop();
                if (vis[u]) {
                    continue;
                }
                vis[u] = true;
                s = t, t = u;
                for (int v : adj[u]) {
                    if (!vis[v]) {
                        auto e = minmax(u, v);
                        sum[v] += cost.at(e);
                        Q.push({sum[v], v});
                    }
                }
            }

            if (best_cost > sum[t])
                best_cost = sum[t], best_cut = {begin(node[t]), begin(node[s])};

            // merge aggregate nodes s, t
            node[s].splice(begin(node[s]), node[t]);

            // move and merge edges from t into s.
            for (int v : adj[t]) {
                auto vs = minmax(v, s), vt = minmax(v, t);
                if (v != s) {
                    if (cost.count(vs)) {
                        cost[vs] += cost.at(vt);
                    } else {
                        cost[vs] = cost.at(vt);
                        adj[s].insert(v);
                        adj[v].insert(s);
                    }
                }
                adj[v].erase(t);
                cost.erase(vt);
            }

            // free t's adjacency list to prevent memory issues.
            unordered_set<int> empty;
            swap(adj[t], empty);
        }

        cut_t cut(best_cut.first, best_cut.second);
        return {best_cost, cut};
    }
};

#endif // MINCUT_HPP
