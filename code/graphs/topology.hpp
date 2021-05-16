#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "../struct/y_combinator.hpp"

auto is_bipartite(int V, const vector<vector<int>>& adj) {
    vector<int8_t> side(V, -1);
    vector<int> bfs;
    int i = 0, S = 0;

    for (int s = 0; s < V; s++) {
        if (side[s] == -1) {
            bfs.push_back(s), S++;
            side[s] = 0;

            while (i < S) {
                int u = bfs[i++];
                for (int v : adj[u]) {
                    if (side[v] < 0) {
                        side[v] = !side[u];
                        bfs.push_back(v), S++;
                    } else if (side[u] == side[v]) {
                        return make_pair(false, side);
                    }
                }
            }
        }
    }

    return make_pair(true, side);
}

template <typename Fn>
auto visit_bridges(const vector<vector<int>>& adj, Fn&& visitor) {
    int V = adj.size();

    vector<int> index(V, 0);
    vector<int> lowlink(V, 0);
    int timer = 1;

    auto dfs = y_combinator([&](auto self, int u, int p) {
        index[u] = lowlink[u] = timer++;
        for (int v : adj[u]) {
            if (v != p) {
                if (index[v]) {
                    lowlink[u] = min(lowlink[u], index[v]);
                } else {
                    self(v, u);
                    lowlink[u] = min(lowlink[u], lowlink[v]);
                    if (lowlink[v] > index[u]) {
                        visitor(u, v);
                    }
                }
            }
        }
    });

    for (int u = 0; u < V; u++) {
        if (!index[u]) {
            dfs(u, -1);
        }
    }
}

template <typename Fn>
auto visit_cut_points(const vector<vector<int>>& adj, Fn&& visitor) {
    int V = adj.size();

    vector<int> index(V, 0);
    vector<int> lowlink(V, 0);
    int timer = 1;

    auto dfs = y_combinator([&](auto self, int u, int p) {
        index[u] = lowlink[u] = timer++;
        int children = 0;
        for (int v : adj[u]) {
            if (v != p) {
                if (index[v]) {
                    lowlink[u] = min(lowlink[u], index[v]);
                } else {
                    self(v, u);
                    lowlink[u] = min(lowlink[u], lowlink[v]);
                    if (lowlink[v] >= index[u] && p != -1) {
                        visitor(u); // non-root cutpoint
                    }
                    children++;
                }
            }
            if (children > 1 && p == -1) {
                visitor(u); // root cutpoint
            }
        }
    });

    for (int u = 0; u < V; u++) {
        if (!index[u]) {
            dfs(u);
        }
    }
}

template <typename Fn>
auto find_cycle(const vector<vector<int>>& adj) {
    int V = adj.size();

    vector<int8_t> color(V, 0);
    vector<int> parent(V, -1);
    array<int, 2> endp = {-1, -1};

    auto dfs = y_combinator([&](auto self, int u, int p) -> bool {
        parent[u] = p;
        color[u] = 1;
        for (int v : adj[u]) {
            if (color[v] == 0 && dfs(v, u)) {
                return true;
            } else if (color[v] == 1) {
                endp = {u, v};
                return true;
            }
        }
        color[u] = 2;
        return false;
    });

    for (int u = 0; u < V; u++) {
        if (color[u] == 0 && dfs(u)) {
            break;
        }
    }

    if (endp[0] != -1) {
        int v = endp[1];
        vector<int> nodes = {v};
        do {
            nodes.push_back(v = parent[v]);
        } while (v != endp[0]);
        reverse(begin(nodes), end(nodes));
        return nodes;
    } else {
        return {};
    }
}

#endif // TOPOLOGY_HPP
