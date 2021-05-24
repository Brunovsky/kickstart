#ifndef EULER_TOUR_TREE_HPP
#define EULER_TOUR_TREE_HPP

#include "../struct/y_combinator.hpp"

auto build_euler_tour_tree_index(const vector<vector<int>>& tree, int root) {
    int V = tree.size();
    vector<array<int, 2>> index(V);
    int timer = 0;

    auto dfs = y_combinator([&](auto self, int u, int p) -> void {
        index[u][0] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
            }
        }
        index[u][1] = timer++;
    });

    dfs(root, -1);
    return index;
}

#endif // EULER_TOUR_TREE_HPP
