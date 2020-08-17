#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Boyer Myrvold planarity test
 * Complexity: O(V) very expensive
 * Based on original paper, jgrapht and boost/graph
 */
struct boyer_myrvold {
    int V, E;
    vector<vector<int>> adj;

    boyer_myrvold(int V) : V(V), E(0) { adj.assign(V, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
        adj[v].push_back(u);
        E++;
    }

    stack<int> merge_stack;
    vector<int> index;
    vector<int> order;
    vector<int> parent;
    vector<int> lowpoint;
    vector<int> vis;
    vector<int> backedge_flag;
    vector<vector<int>> pertinent_roots;
    vector<list<int>> separated_dfs_child_list;
    vector<list<int>::iterator> representative;
    int depth = 0;

    void dfs(int u, int p = -1) {
        index[u] = depth;
        order[depth] = u;
        parent[u] = p;
        lowpoint[u] = index[u];
        depth++;

        for (int v : adj[u]) {
            if (index[v] == -1) {
                dfs(v, u);
                lowpoint[u] = min(lowpoint[u], lowpoint[v]);
            } else if (index[v] < index[u]) {
                lowpoint[u] = min(lowpoint[u], index[v]);
            }
        }
    }

    void prepare() {
        merge_stack = stack<int>();
        index.assign(V, -1);
        order.assign(V, -1);
        parent.assign(V, -1);
        lowpoint.assign(V, -1);
        vis.assign(V, -1);
        backedge_flag.assign(V, -1);
        pertinent_roots.assign(V, {});
        separated_dfs_child_list.assign(V, {});
        representative.assign(V, {});
        depth = 0;

        for (int u = 0; u < V; u++) {
            if (index[u] == -1) {
                dfs(u);
            }
        }

        for (int i = 0; i < V; i++) {
            int u = order[i], p = parent[u];
            if (p != -1) {
                separated_dfs_child_list[p].push_back(u);
            }
        }
    }

    bool pertinent(int w) {
        return backedge_flag[w] != -1 || !pertinent_roots[w].empty();
    }
    bool inactive(int w, int v) { //
        return !pertinent(w) && !externally_active(w, v);
    }
    bool internally_active(int w, int v) {
        return pertinent(w) && !externally_active(w, v);
    }
    bool externally_active(int w, int v) {
        return parent[w] < v || (!separated_dfs_child_list.empty() &&
                                 lowpoint[separated_dfs_child_list[w].front()] < v);
    }
    bool is_root(int w) { return w == lowpoint[w]; }

    void walkup(int w, int v) {
        backedge_flag[w] = v;
        int x = w, xin = 1;
        int y = w, yin = 0;

        while (x != y && vis[x] != v && vis[y] != v) {
            vis[x] = vis[y] = v;
        }
    }

    bool is_planar() {
        if (V <= 3) {
            return true;
        }
        if (E > 3 * V - 6) {
            return false;
        }
        return true;
    }
};
