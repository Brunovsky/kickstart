#ifndef MINCOST_HUNGARIAN_HPP
#define MINCOST_HUNGARIAN_HPP

#include "../struct/integer_heaps.hpp" // pairing_int_heap

/**
 * Min-cost perfect bipartite matching (hungarian, dijkstra-based, nonnegative costs)
 * Based on: https://web.stanford.edu/class/cs261/min_cost_bipartite_matching.pdf
 * Complexity: O(EW log E), W=max(U,V).
 *
 * Need pairing_int_heap for the internal dijkstra.
 *
 * If the bipartite graph is not balanced (U != V) then it must be padded.
 * Usually the problem you're trying to solve will require a specific type of padding.
 *
 * If the graph is not too imbalanced, or it is actually complete but imbalanced, then
 * pad_complete(w) will add |U-V|W extra edges with cost w (usually w should be 0). There
 * may be quadratically many such edges.
 * If a perfect matching did not exist, it will still not exist.
 *
 * Alternatively, pad_reverse() will set W=U+V and add a flipped copy of the graph to
 * itself plus U+V linking edges. The resulting graph will have a perfect matching, with
 * double the cost.
 * Some linking edges have weight 0 and some have weight inf; this infinity must be
 * sufficiently big for the algorithm to prefer any other path, which might require
 * promoting Cost's type (from int to long).
 *
 * If padding was performed, you should specify if you want the minimum cost to include
 * padding or not in the call to the compute function. In most cases you want to discard
 * this extra cost; if you used pad_complete(w) with w>0 you probably want to keep them.
 */
template <typename Cost = long, typename CostSum = Cost>
struct mincost_hungarian {
    int U = 0, V = 0, W = 0, E = 0;
    bool padded = false;
    vector<vector<pair<int, Cost>>> adj;
    vector<int> m[2];

    mincost_hungarian() = default;
    mincost_hungarian(int U, int V) : U(U), V(V), W(max(U, V)), adj(W) {}

    void add(int u, int v, Cost w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        adj[u].push_back({v, w});
    }

    void pad_complete(Cost w) {
        assert(!padded), padded = true;
        for (int v = V; v < U; v++) // edges if padding V side
            for (int u = 0; u < U; u++)
                adj[u].push_back({v, w}), E++;
        for (int u = U; u < V; u++) // edges if padding U side
            for (int v = 0; v < V; v++)
                adj[u].push_back({v, w}), E++;
    }

    void pad_reverse(Cost badw = costinf, bool guarantee = false) {
        assert(!padded), padded = true, W = U + V;
        adj.resize(W, {});
        for (int u = 0; u < U; u++)
            for (auto [v, w] : adj[u])
                adj[v + U].push_back({u + V, w});
        if (U <= V) {
            for (int v = 0; v < V; v++)
                adj[v + U].push_back({v, 0});
            for (int u = 0; u < U && guarantee; u++)
                adj[u].push_back({u + V, badw});
        } else {
            for (int u = 0; u < U; u++)
                adj[u].push_back({u + V, 0});
            for (int v = 0; v < V && guarantee; v++)
                adj[v + U].push_back({v, badw});
        }
    }

    vector<int> prev[2];
    vector<CostSum> pi[2], dist[2];
    pairing_int_heap<less_container<vector<CostSum>>> heap;
    static inline constexpr Cost costinf = numeric_limits<Cost>::max() / 3;
    static inline constexpr CostSum costsuminf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra() {
        dist[0].assign(W + 1, costsuminf);
        dist[1].assign(W, costsuminf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        for (int u = 0; u < W; u++)
            if (m[0][u] == W)
                dist[0][u] = 0, heap.push(u);

        while (!heap.empty()) {
            int u = heap.pop();
            if (u == W) {
                continue;
            }
            for (auto [v, w] : adj[u]) {
                int y = m[1][v];
                CostSum relaxed = min(dist[0][u] + w + pi[0][u] - pi[1][v], costsuminf);
                if (dist[0][y] > relaxed) {
                    dist[0][y] = relaxed, prev[0][y] = v;
                    heap.push_or_improve(y);
                }
                if (dist[1][v] > relaxed) {
                    dist[1][v] = relaxed, prev[1][v] = u;
                }
            }
        }
        reprice();
        return prev[0][W] != -1;
    }

    void reprice() {
        for (int i : {0, 1}) {
            for (int u = 0; u < W; u++) {
                pi[i][u] = min(dist[i][u] + pi[i][u], costsuminf);
            }
        }
    }

    auto path() const {
        vector<array<int, 2>> nodes;
        int v = prev[0][W];
        while (v != -1) {
            nodes.push_back({prev[1][v], v});
            v = prev[0][prev[1][v]];
        }
        return nodes;
    }

    auto mincost_max_matching(bool include_padding = false) {
        assert(U == V || padded);
        m[0].assign(W, W);
        m[1].assign(W, W);
        pi[0].assign(W, 0);
        pi[1].assign(W, 0);
        heap = pairing_int_heap<less_container<vector<CostSum>>>(W + 1, dist[0]);

        int matches = 0;
        while (matches < W && dijkstra()) {
            for (auto [u, v] : path()) {
                m[0][u] = v, m[1][v] = u;
            }
            matches++;
        }
        if (matches < W) {
            return CostSum(-1);
        }

        CostSum min_cost = 0;
        for (int u = 0; u < W; u++) {
            int v = m[0][u];
            assert(0 <= v && v < W);
            if (include_padding || (u < U && v < V)) {
                for (auto [t, w] : adj[u]) {
                    if (v == t) {
                        min_cost += w;
                        break;
                    }
                }
            }
        }
        return min_cost;
    }
};

#endif // MINCOST_HUNGARIAN_HPP
