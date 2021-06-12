#pragma once

#include "../struct/y_combinator.hpp"

/**
 * Centroid decomposition (1-indexed)
 */
auto build_tree_centroid_decomposition(const vector<vector<int>>& tree) {
    int V = tree.size();
    vector<int> subsize(V);
    vector<int> cparent(V);
    vector<int> parent(V);
    vector<int> bfs(V);
    vector<bool> mark(V, false);

    for (int s = 1; s < V; s++) {
        while (!mark[s]) {
            parent[s] = -1;
            bfs[0] = s;
            int i = 0, S = 1, link = -1;

            while (i < S) {
                int u = bfs[i++];
                for (int v : tree[u]) {
                    if (v != parent[u] && !mark[v]) {
                        parent[v] = u;
                        bfs[S++] = v;
                    } else if (mark[v]) {
                        link = v;
                    }
                }
            }

            for (i = S - 1; i >= 0; i--) {
                int u = bfs[i];
                subsize[u] = 1;
                bool is = true;
                for (int v : tree[u]) {
                    if (v != parent[u] && !mark[v]) {
                        subsize[u] += subsize[v];
                        is &= subsize[v] <= S / 2;
                    }
                }
                is &= S - subsize[u] <= S / 2;
                if (is) {
                    mark[u] = true;
                    cparent[u] = link;
                    break;
                }
            }

            assert(i >= 0);
        }
    }

    return cparent;
}
