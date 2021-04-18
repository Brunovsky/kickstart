#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "../hash.hpp"

/**
 * Determine if graph is bipartite
 * Nodes 0-indexed
 * Complexity: O(V + E)
 */
bool is_bipartite(int V, const vector<vector<int>>& adj) {
    vector<int8_t> side(V, -1);
    vector<int> bfs;
    int i = 0, S = 0;

    for (int s = 0; s < V; s++) {
        if (side[s] >= 0)
            continue;

        bfs.push_back(s), S++;
        side[s] = 0;

        while (i < S) {
            int u = bfs[i++];
            for (int v : adj[u]) {
                if (side[v] < 0) {
                    side[v] = !side[u];
                    bfs.push_back(v), S++;
                } else if (side[u] == side[v]) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Find cycles in a graph
 * Nodes 0-indexed
 * Complexity: O(V + E)
 * Source: https://cp-algorithms.com/graph/finding-cycle.html
 */
struct cycle_finder {
    int V;
    vector<vector<int>> out; // edges u->v or u--v
    vector<int8_t> color;
    vector<int> parent;
    array<int, 2> endp;

    bool dfs_cycle(int u, int p = -1) {
        parent[u] = p;
        color[u] = 1;
        for (int v : out[u]) {
            if (color[v] == 0 && dfs_cycle(v, u)) {
                return true;
            } else if (color[v] == 1) {
                endp = {u, v};
                return true;
            }
        }
        color[u] = 2;
        return false;
    }

    void find_cycle() {
        color.assign(V, 0);
        parent.assign(V, -1);

        for (int u = 0; u < V; u++) {
            if (color[u] == 0 && dfs_cycle(u)) {
                break;
            }
        }
    }

    auto path() const {
        vector<int> nodes;
        for (int v = endp[1]; v != endp[0]; v = parent[v])
            nodes.push_back(v);
        nodes.push_back(endp[0]);
        reverse(begin(nodes), end(nodes));
        return nodes;
    }
};

/**
 * Find bridges in the graph
 * Nodes 0-indexed
 * Complexity: O(V + E)
 * Source: https://cp-algorithms.com/graph/bridge-searching.html
 */
struct bridges_searcher {
    int V;
    vector<vector<int>> adj;       // edges u--v
    vector<array<int, 2>> bridges; // bridges u--v
    vector<int> index, lowlink;
    int timer = 0;

    void dfs_bridges(int u, int p = -1) {
        index[u] = lowlink[u] = timer++;
        for (int v : adj[u]) {
            if (v == p)
                continue;
            if (index[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            } else {
                dfs_bridges(v, u);
                lowlink[u] = min(lowlink[u], lowlink[v]);
                if (lowlink[v] > index[u]) {
                    bridges.push_back({u, v}); // found a bridge u->v
                }
            }
        }
    }

    void find_bridges() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        bridges.clear();
        timer = 1;

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                dfs_bridges(u);
            }
        }
    }
};

/**
 * Find strongly connected components in reverse topological order (Tarjan)
 * Nodes 0-indexed
 * Complexity: O(V + E), same for condensation
 */
struct strongly_connected_components {
    int V, C = 0;                        // C: number of scc
    vector<vector<int>> out;             // edges u->v
    vector<vector<int>> cset;            // cset[c]: nodes in scc c
    vector<int> cmap;                    // cmap[u]: component of node u
    vector<vector<int>> cadjout, cadjin; // condensation graph
    vector<int> index, lowlink;
    stack<int> S;
    int timer;

    void dfs_tarjan(int u) {
        index[u] = lowlink[u] = timer++;
        S.push(u);

        for (int v : out[u]) {
            if (index[v] && cmap[v] == -1) { // <-- skip cmap'd nodes
                lowlink[u] = min(lowlink[u], index[v]);
            } else if (!index[v]) {
                dfs_tarjan(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            }
        }

        // found a strongly connected components
        if (index[u] == lowlink[u]) {
            int c = C++;
            cset.push_back({});
            int v;
            do {
                v = S.top(), S.pop();
                cmap[v] = c, cset[c].push_back(v);
            } while (u != v);
        }
    }

    void find_scc() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        S = stack<int>();
        cset.clear();
        cmap.assign(V, -1);
        timer = 1, C = 0;

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                dfs_tarjan(u);
            }
        }
    }

    void condensate() {
        // use an unordered map etc if you need to quantify incidence between scc
        unordered_set<array<int, 2>> cedges;

        for (int u = 0; u < V; u++) {
            for (int v : out[u]) {
                if (cmap[u] != cmap[v]) {
                    cedges.insert({cmap[u], cmap[v]});
                }
            }
        }

        cadjout.assign(C, {}), cadjin.assign(C, {});
        for (auto [u, v] : cedges) {
            cadjout[u].push_back(v), cadjin[v].push_back(u);
        }
    }
};

/**
 * Find articulation/cut points in the graph
 * Nodes 0-indexed
 * Complexity: O(V + E)
 * Source: https://cp-algorithms.com/graph/cutpoints.html
 */
struct cut_points {
    int V;
    vector<vector<int>> adj; // edges u--v
    vector<bool> is_cutpoint;
    vector<int> cutpoints;
    vector<int> index, lowlink;
    int timer;

    void dfs_cut_points(int u, int p = -1) {
        index[u] = lowlink[u] = timer++;
        int children = 0;
        for (int v : adj[u]) {
            if (v == p)
                continue;
            if (index[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            } else {
                dfs_cut_points(v, u);
                lowlink[u] = min(lowlink[u], lowlink[v]);
                if (lowlink[v] >= index[u] && p != -1) {
                    is_cutpoint[u] = true, cutpoints.push_back(u); // found cutpoint u
                }
                children++;
            }
        }
        if (children > 1 && p == -1) {
            is_cutpoint[u] = true, cutpoints.push_back(u); // found root cutpoint u
        }
    }

    void find_cut_points() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        is_cutpoint.assign(V, false);
        cutpoints.clear();
        timer = 1;

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                dfs_cut_points(u);
            }
        }
    }
};

/**
 * Compute the dominator tree of a directed graph
 * Nodes 1-indexed
 * Complexity: O((V + E) log(V))
 * Source:
 *     "A Fast Algorithm for Finding Dominators in a Flow Graph", T. Lengauer, R. Tarjan
 *      https://tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
 */
struct dominator_tree {
    int V;
    vector<vector<int>> out, in; // edges u->v
    vector<int> dom;             // dom[u]: immediate dominator of u (when done)
    vector<int> parent, semi, vertex, ancestor, label;
    int timer = 0;

    void dfs_index(int u) {
        vertex[timer] = u, semi[u] = timer++;
        for (int v : out[u]) {
            if (semi[v] == 0) {
                parent[v] = u;
                dfs_index(v);
            }
        }
    }

    void compress(int v) {
        if (ancestor[ancestor[v]] != 0) {
            compress(ancestor[v]);
            if (semi[label[v]] > semi[label[ancestor[v]]]) {
                label[v] = label[ancestor[v]];
            }
            ancestor[v] = ancestor[ancestor[v]];
        }
    }

    int eval(int v) {
        if (ancestor[v] == 0) {
            return v;
        } else {
            compress(v);
            return label[v];
        }
    }

    void build_dominator_tree(int root = 1) {
        dom.assign(V + 1, 0);
        parent.assign(V + 1, 0);
        semi.assign(V + 1, 0);
        vertex.assign(V + 1, 0);
        ancestor.assign(V + 1, 0);
        label.assign(V + 1, 0), iota(begin(label), end(label), 0);
        timer = 1;

        dfs_index(root);

        vector<int> bucket_head(V + 1, 0), bucket_next(V + 1, 0);

        for (int i = V; i >= 2; i--) {
            int w = vertex[i];
            for (int v : in[w]) {
                int u = eval(v);
                semi[w] = min(semi[w], semi[u]);
            }
            // push w onto the front of bucket b
            int b = vertex[semi[w]];
            bucket_next[w] = bucket_head[b], bucket_head[b] = w;
            ancestor[w] = parent[w]; // link
            // visit all nodes in bucket parent[w]
            for (int v = bucket_head[parent[w]]; v != 0; v = bucket_next[v]) {
                int u = eval(v);
                dom[v] = semi[u] < semi[v] ? u : parent[w];
            }
            bucket_head[parent[w]] = 0;
        }

        for (int i = 2; i <= V; i++) {
            int w = vertex[i];
            if (dom[w] != vertex[semi[w]]) {
                dom[w] = dom[dom[w]];
            }
        }

        dom[root] = 0;
    }
};

#endif // TOPOLOGY_HPP
