#include "../struct/y_combinator.hpp"

#define AUTO [[maybe_unused]] auto

/**
 * A repertoir of techniques on a rooted tree with values on nodes
 * Assumes tree is 1-indexed so 0 can be used nicely.
 */
void rooted_tree_sample(vector<vector<int>> adj, int root, vector<int> arr) {
    int N = adj.size() - 1;

    // ***** General
    vector<int> parent(N + 1);  // parent of node
    vector<int> depth(N + 1);   // depth of node from root (depth[root]=0)
    vector<int> deepest(N + 1); // depth of deepest node in subtree
    vector<int> tin(N + 1);     // time we enter node
    vector<int> tout(N + 1);    // time we leave node
    vector<int> subsize(N + 1); // size of subtree
    vector<int> heavy(N + 1);   // heaviest child
    int timer = 0;

    y_combinator([&](auto self, int u, int p) -> void {
        tin[u] = timer++;

        parent[u] = p;
        deepest[u] = depth[u];
        subsize[u] = 1;

        for (int v : adj[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                self(v, u);
                deepest[u] = max(deepest[u], deepest[v]);
                subsize[u] += subsize[v];
                if (subsize[v] > subsize[heavy[u]]) {
                    heavy[u] = v;
                }
            }
        }

        tout[u] = timer;
    })(root, 0);

    // Maximum depth of the tree
    int D = 1 + deepest[root];
    const int Db = D > 1 ? 1 + 8 * sizeof(D) - __builtin_clz(D - 1) : 1;

    // Reverse of tin
    assert(timer == N);
    vector<int> tin_who(N);
    for (int u = 1; u <= N; u++) {
        tin_who[tin[u]] = u;
    }

    // ***** Euler Tour
    vector<array<int, 2>> euler_index(N + 1); // euler tour index, [0]=in [1]=out
    vector<int> euler_tour;
    int euler_timer = 0;

    y_combinator([&](auto self, int u, int p) -> void {
        euler_index[u][0] = euler_timer++;
        euler_tour.push_back(u);

        for (int v : adj[u]) {
            if (v != p) {
                self(v, u);
            }
        }

        euler_index[u][1] = euler_timer++;
        euler_tour.push_back(u);
    })(root, 0);

    // Reverse of euler tour index
    vector<int> euler_who(2 * N);
    for (int u = 1; u <= N; u++) {
        euler_who[euler_index[u][0]] = u;
        euler_who[euler_index[u][1]] = u;
    }

    // ***** LCA sparse table: Compute lca jumps for fast LCA and ancestor jumps
    vector<vector<int>> up(Db, vector<int>(N + 1, 0));
    up[0] = parent;
    for (int b = 1; b < Db; b++) {
        for (int u = 1; u <= N; u++) {
            if (up[b - 1][u] != 0) {
                up[b][u] = up[b - 1][up[b - 1][u]];
            }
        }
    }
    AUTO get_ancestor = [&](int u, int steps) { // O(log N)
        for (int b = Db - 1; b >= 0; b--) {
            if (steps & (1 << b)) {
                u = up[b][u];
            }
        }
        return u;
    };
    AUTO get_lca = [&](int u, int v) { // O(log N)
        if (depth[u] < depth[v]) {
            v = get_ancestor(v, depth[v] - depth[u]);
        } else if (depth[u] > depth[v]) {
            u = get_ancestor(u, depth[u] - depth[v]);
        }
        for (int b = Db - 1; b >= 0; b--) {
            if (up[b][u] != up[b][v]) {
                u = up[b][u], v = up[b][v];
            }
        }
        return u == v ? u : up[0][u];
    };
    AUTO get_dist = [&](int u, int v) { // O(log N)
        return depth[u] + depth[v] - 2 * depth[get_lca(u, v)];
    };

    // ***** Timing-ordered depth lists
    vector<vector<int>> on_depth(D);
    vector<vector<int>> on_depth_tin(D);
    vector<int> on_depth_index(N + 1);

    for (int i = 0; i < N; i++) {
        int u = tin_who[i];
        on_depth_index[u] = on_depth[depth[u]].size();
        on_depth[depth[u]].push_back(u);
        on_depth_tin[depth[u]].push_back(i);
    }

    AUTO query_depth_bounds = [&](int u, int d) -> array<int, 2> { // O(log N)
        assert(depth[u] <= d && d <= deepest[u]);
        const auto& on = on_depth_tin[d];
        auto liter = lower_bound(begin(on), end(on), tin[u]);
        auto riter = upper_bound(rbegin(on), rend(on), tout[u], greater<int>{});
        int l = liter - begin(on);
        int r = on.size() - 1 - (riter - rbegin(on));
        return {l, r};
    };

    // ***** Bfs-timing-ordered list
    vector<int> bfs_order(N), bfs_index(N + 1);

    for (int i = 0, d = 0; d < D; d++) {
        for (int u : on_depth[d]) {
            bfs_index[u] = i;
            bfs_order[i++] = u;
        }
    }

    // ***** Heavy-light decomposition
    vector<int> heavy_head(N + 1);
    vector<int> heavy_in(N + 1);
    vector<int> heavy_out(N + 1);
    int heavy_timer = 0;

    // Trick to allow subtree queries, move heavy[u] to adj[u][0] for all u
    for (int u = 1; u <= N; u++) {
        for (int i = 1, U = adj[u].size(); i < U; i++) {
            if (adj[u][i] == heavy[u]) {
                swap(adj[u][0], adj[u][i]);
                break;
            }
        }
    }

    y_combinator([&](auto self, int u, int h, int p) -> void {
        heavy_head[u] = h;
        heavy_in[u] = heavy_timer++;
        if (heavy[u]) {
            self(heavy[u], h, u);
        }
        for (int v : adj[u]) {
            if (v != p && v != heavy[u]) {
                self(v, v, u);
            }
        }
        heavy_out[u] = heavy_timer++;
    })(root, root, 0);

    // Build the heavy segtree. Sample without updates.
    {
        struct HeavyNode {
            array<int, 2> range;
            // ...
            int val;
        };

        int N2 = 1;
        while (N2 < N) {
            N2 <<= 1;
        }
        vector<HeavyNode> segtree(2 * N);
        for (int i = 0; i < N; i++) {
            segtree[i + N].range = {i, i};
            // ...
            segtree[i + N].val = arr[i + 1];
        }
        rotate(begin(segtree) + N, begin(segtree) + (3 * N - N2), end(segtree));
        for (int i = N - 1; i >= 1; i--) {
            const auto& cl = segtree[i << 1];
            const auto& cr = segtree[i << 1 | 1];
            segtree[i].range = {cl.range[0], cr.range[1]};
            // ...
            segtree[i].val = cl.val + cr.val;
        }

        // O(log N) segtree query
        AUTO query_segtree = y_combinator([&](auto self, int i, int l, int r) -> int {
            if (r < segtree[i].range[0] || segtree[i].range[1] < l) {
                return 0;
            }
            if (l <= segtree[i].range[0] && segtree[i].range[1] <= r) {
                return segtree[i].val;
            }
            auto ql = self(i << 1, l, r);
            auto qr = self(i << 1 | 1, l, r);
            return ql + qr; // merge
        });
        AUTO query_path = [&](int u, int v) { // O(log^2 N) heavy query
            int ans = 0;
            while (heavy_head[u] != heavy_head[v]) {
                if (depth[heavy_head[u]] > depth[heavy_head[v]]) {
                    auto q = query_segtree(1, heavy_in[heavy_head[u]], heavy_in[u]);
                    ans = ans + q; // merge
                    u = parent[heavy_head[u]];
                } else {
                    auto q = query_segtree(1, heavy_in[heavy_head[v]], heavy_in[v]);
                    ans = ans + q; // merge
                    v = parent[heavy_head[v]];
                }
            }
            if (depth[u] > depth[v]) {
                auto q = query_segtree(1, heavy_in[v], heavy_in[u]);
                ans = ans + q; // merge
            } else {
                auto q = query_segtree(1, heavy_in[u], heavy_in[v]);
                ans = ans + q; // merge
            }
            return ans;
        };
    }

    // ***** Centroid decomposition
    vector<int> centroid_parent(N + 1);
    vector<int> centroid_subsize(N + 1);

    y_combinator([&](auto self, int S, int u, int p, int cp, auto&& mark) -> bool {
        centroid_subsize[u] = 1;
        bool is = true;
        for (int v : adj[u]) {
            if (v != p && !mark[v]) {
                if (self(S, v, u, cp, mark)) {
                    return true;
                } else {
                    subsize[u] += subsize[v];
                    is &= subsize[v] <= S / 2;
                }
            }
        }
        if (is && S - subsize[u] <= S / 2) {
            mark[u] = true;
            centroid_parent[u] = cp;
            for (int v : adj[u]) {
                if (v != p && !mark[v]) {
                    self(subsize[v], v, u, u, mark);
                } else if (v == p && !mark[v]) {
                    self(S - subsize[u], v, u, u, mark);
                }
            }
            return true;
        }
        return false;
    })(N, root, 0, 0, vector<bool>(N + 1, false));
}
