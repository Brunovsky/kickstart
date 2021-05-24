#ifndef HEAVY_LIGHT_DECOMPOSITION
#define HEAVY_LIGHT_DECOMPOSITION

#include "../struct/y_combinator.hpp"

auto build_tree_heavy_light_decomposition(const vector<vector<int>>& tree, int root) {
    int V = tree.size();
    vector<int> subsize(V);
    vector<int> heavy(V, -1);
    vector<int> parent(V);
    vector<int> depth(V);
    vector<int> head(V);
    vector<int> index(V);
    int timer = 0;

    auto dfs = y_combinator([&](auto self, int u, int p) -> void {
        subsize[u] = 1;
        int largest = 0;
        for (int v : tree[u]) {
            if (v != p) {
                parent[v] = u;
                depth[v] = depth[u] + 1;
                self(v, u);
                subsize[u] += subsize[v];
                if (largest < subsize[v]) {
                    largest = subsize[v];
                    heavy[u] = v;
                }
            }
        }
    });

    auto decompose = y_combinator([&](auto self, int u, int h, int p) -> void {
        head[u] = h;
        index[u] = timer++;
        if (heavy[u] != -1) {
            self(heavy[u], h, u);
        }
        for (int v : tree[u]) {
            if (v != p && v != heavy[u]) {
                self(v, v, u);
            }
        }
    });

    dfs(root, -1);
    decompose(root, root, -1);

    return make_tuple(move(parent), move(depth), move(head), move(index));
}

#endif // HEAVY_LIGHT_DECOMPOSITION
