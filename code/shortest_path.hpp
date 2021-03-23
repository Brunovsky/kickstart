#ifndef SHORTEST_PATH_HPP
#define SHORTEST_PATH_HPP

#include "graph.hpp"
#include "integer_heaps.hpp"

// *****

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 * Complexity: O(E + V log(V))
 */
struct dijkstra {
    int V, E = 0;
    vector<vector<pair<int, long>>> adj;

    dijkstra(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), adj(V) {
        for (int e = 0; e < E; e++) {
            assert(weight[e] >= 0);
            auto [u, v] = g[e];
            adj[u].push_back({v, weight[e]});
        }
    }

    vector<long> dist;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 2;

    long compute(int s, int t = -1) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = 0, prev[s] = s;

        auto cmp = [&](int u, int v) { return dist[u] < dist[v]; };
        pairing_int_heap Q(V, cmp);
        Q.push(s);

        while (!Q.empty()) {
            auto u = Q.pop();
            if (u == t) {
                return dist[t];
            }
            for (auto [v, w] : adj[u]) {
                long cost = dist[u] + w;
                if (dist[v] > cost) {
                    dist[v] = cost;
                    prev[v] = u;
                    Q.push_or_improve(v);
                }
            }
        }
        return inf;
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
 * Complexity: O(E + V log(V))
 */
struct astar {
    int V, E = 0;
    vector<vector<pair<int, long>>> adj;
    using heuristic_t = const function<long(int)>&;

    astar(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(size(g)), adj(V) {
        for (int e = 0; e < E; e++) {
            assert(weight[e] >= 0);
            auto [u, v] = g[e];
            adj[u].push_back({v, weight[e]});
        }
    }

    vector<long> dist, heur;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 2;

    long compute(int s, int t, heuristic_t h) {
        dist.assign(V, inf);
        heur.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = heur[s] = 0, prev[s] = s;

        auto cmp = [&](int u, int v) { return heur[u] < heur[v]; };
        pairing_int_heap Q(V, cmp);
        Q.push(s);

        while (!Q.empty()) {
            int u = Q.pop();
            if (u == t) {
                return dist[t];
            }
            for (auto [v, w] : adj[u]) {
                long cost = dist[u] + w;
                if (dist[v] > cost) {
                    dist[v] = cost;
                    heur[v] = cost + h(v);
                    prev[v] = u;
                    Q.push_or_improve(v);
                }
            }
        }
        return inf;
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
 * Negative loops are detected
 * Complexity: O(V^3)
 */
struct floyd_warshall {
    int V, E = 0;
    vector<tuple<int, int, long>> edge;

    floyd_warshall(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(g.size()), edge(g.size()) {
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[e] = {u, v, weight[e]};
        }
    }

    vector<vector<long>> dist;
    vector<vector<int>> next;
    static inline constexpr long inf = LONG_MAX / 2;

    void compute() {
        dist.assign(V, vector<long>(V, inf));
        next.assign(V, vector<int>(V, -1));

        for (auto [u, v, w] : edge) {
            dist[u][v] = min(w, dist[u][v]);
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
                if (dist[u][u] < 0) {
                    throw runtime_error("floyd_warshall: negative cycle detected");
                }
            }
        }
        fix_infinities();
    }

    void fix_infinities() {
        for (int u = 0; u < V; u++) {
            for (int v = 0; v < V; v++) {
                if (dist[u][v] > inf / 2)
                    dist[u][v] = inf;
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
 * For undirected graphs insert edges both ways
 * Complexity: O(EV)
 */
struct bellman_ford {
    int V, E = 0;
    vector<tuple<int, int, long>> edge;

    bellman_ford(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(g.size()), edge(g.size()) {
        assert(weight.size() == E);
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[e] = {u, v, weight[e]};
        }
    }

    vector<long> dist;
    vector<int> prev;
    static inline constexpr long inf = LONG_MAX / 2;

    void compute(int s) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        dist[s] = 0, prev[s] = s;

        bool stop = false;
        for (int k = 1; k <= V && !stop; k++) {
            stop = true;
            for (auto [u, v, w] : edge) {
                if (dist[v] > dist[u] + w && dist[u] < inf) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                    stop = false;
                }
            }
        }

        if (!stop) {
            throw runtime_error("bellman_ford: negative cycle detected");
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
 * Negative loops are detected
 * Complexity: O(EV + V^2 log(V))
 */
struct johnsons {
    int V, E = 0;
    vector<tuple<int, int, long>> edge;
    vector<int> off;

    johnsons(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(g.size()), edge(g.size()), off(V + 1, 0) {
        for (auto [u, _] : g) {
            off[u + 1]++;
        }
        inclusive_scan(begin(off), end(off), begin(off));
        auto cur = off;
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[cur[u]++] = {u, v, weight[e]};
        }
    }

    vector<vector<long>> dist;
    vector<vector<int>> prev;
    vector<long> pi;
    static inline constexpr long inf = LONG_MAX / 2;

    void bellman_ford() {
        pi.assign(V, 0);

        bool stop = false;
        for (int k = 1; k <= V && !stop; k++) {
            stop = true;
            for (auto [u, v, w] : edge) {
                if (pi[v] > pi[u] + w) {
                    pi[v] = pi[u] + w;
                    stop = false;
                }
            }
        }

        if (!stop) {
            throw runtime_error("johnsons: negative cycle detected");
        }
    }

    void dijkstra(int s) {
        dist[s].assign(V, inf);
        prev[s].assign(V, -1);
        dist[s][s] = 0, prev[s][s] = s;

        auto cmp = [&](int u, int v) { return dist[s][u] < dist[s][v]; };
        pairing_int_heap Q(V, cmp);
        Q.push(s);

        while (!Q.empty()) {
            int u = Q.pop();
            for (int i = off[u]; i < off[u + 1]; i++) {
                auto [x, v, w] = edge[i];
                if (dist[s][v] > dist[s][u] + pi[u] - pi[v] + w) {
                    dist[s][v] = dist[s][u] + pi[u] - pi[v] + w;
                    prev[s][v] = u;
                    Q.push_or_improve(v);
                }
            }
        }
        for (int u = 0; u < V; u++) {
            if (prev[s][u] == -1) {
                assert(dist[s][u] == inf);
            } else {
                dist[s][u] += pi[u] - pi[s];
            }
        }
    }

    void compute() {
        bellman_ford();
        dist.assign(V, {});
        prev.assign(V, {});
        for (int u = 0; u < V; u++)
            dijkstra(u);
    }

    auto path(int s, int v) {
        vector<int> path;
        if (prev[s][v] == -1) {
            return path;
        }
        int t;
        do {
            path.push_back(v);
            t = v;
            v = prev[s][v];
        } while (t != v);
        reverse(begin(path), end(path));
        return path;
    }
};

/**
 * Implementation for directed graphs
 * For undirected graphs insert edges both ways
 * Negative loops are detected
 * Complexity: O(EV)
 */
struct goldberg_radzik {
    int V, E = 0;
    vector<pair<int, long>> edge;
    vector<int> off;

    goldberg_radzik(int V, const edges_t& g, const vector<long>& weight)
        : V(V), E(g.size()), edge(g.size()), off(V + 1, 0) {
        for (auto [u, _] : g) {
            off[u + 1]++;
        }
        inclusive_scan(begin(off), end(off), begin(off));
        auto cur = off;
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            edge[cur[u]++] = {v, weight[e]};
        }
    }

    vector<long> dist;
    vector<int> prev, A, B;
    vector<int8_t> label;
    static inline constexpr long inf = LONG_MAX / 2;
    static inline constexpr int8_t not_in = 0, in_b = 1, in_a = 2;

    void dfs(int u) {
        // assert(label[u] == in_b || label[u] == in_a);
        if (label[u] == in_b) {
            label[u] = not_in;
            for (int e = off[u]; e < off[u + 1]; e++) {
                auto [v, w] = edge[e];
                if (dist[v] > dist[u] + w) {
                    label[u] = in_a;
                    break;
                }
            }
            if (label[u] == not_in)
                return;
        }
        label[u] = in_a;
        for (int e = off[u]; e < off[u + 1]; e++) {
            auto [v, w] = edge[e];
            if (dist[v] > dist[u] + w && label[v] != in_a) {
                dfs(v);
            }
        }
        A.push_back(u);
    }

    void compute(int s) {
        dist.assign(V, inf);
        prev.assign(V, -1);
        label.assign(V, not_in);
        A = {}, B = {s};
        dist[s] = 0, prev[s] = -1, label[s] = in_b;
        int rounds = 0;

        while (!B.empty() && rounds++ < V) {
            do {
                int u = B.back();
                B.pop_back();
                if (label[u] == in_b) {
                    dfs(u);
                }
            } while (!B.empty());

            while (!A.empty()) {
                int u = A.back();
                A.pop_back();
                label[u] = not_in;
                for (int e = off[u]; e < off[u + 1]; e++) {
                    auto [v, w] = edge[e];
                    if (dist[v] > dist[u] + w) {
                        dist[v] = dist[u] + w;
                        prev[v] = u;
                        if (label[v] != in_b) {
                            label[v] = in_b;
                            B.push_back(v);
                        }
                    }
                }
            }
        }

        if (rounds > V) {
            throw runtime_error("goldberg_radzik: negative cycle detected");
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

#endif // SHORTEST_PATH_HPP
