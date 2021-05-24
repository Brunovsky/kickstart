#ifndef CENTROID_DECOMPOSITION_HPP
#define CENTROID_DECOMPOSITION_HPP

#include "../struct/y_combinator.hpp"

auto build_tree_centroid_decomposition(const vector<vector<int>>& tree) {
    int V = tree.size();
    vector<int> subsize(V);
    vector<int> cparent(V);
    vector<bool> mark(V, false);
    int marked = 0;

    auto dfs = y_combinator([&](auto self, int S, int u, int p, int cp) -> bool {
        subsize[u] = 1;
        bool is = true;
        for (int v : tree[u]) {
            if (v != p && !mark[v]) {
                if (self(S, v, u, cp)) {
                    return true;
                } else {
                    subsize[u] += subsize[v];
                    is &= subsize[v] <= S / 2;
                }
            }
        }
        if (is && S - subsize[u] <= S / 2) {
            mark[u] = true, cparent[u] = cp, marked++;
            for (int v : tree[u]) {
                if (v != p && !mark[v]) {
                    self(subsize[v], v, u, u);
                } else if (v == p && !mark[v]) {
                    self(S - subsize[u], v, u, u);
                }
            }
            return true;
        }
        return false;
    });

    dfs(V, 0, -1, -1);
    assert(marked == V);
    return cparent;
}

#endif // CENTROID_DECOMPOSITION_HPP
