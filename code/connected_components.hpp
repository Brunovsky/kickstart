#include <bits/stdc++.h>

using namespace std;

// *****

// Tarjan's
struct strongly_connected_components {
    int V;
    vector<vector<int>> adj;

    strongly_connected_components(int V) : V(V) {
        adj.resize(V, {});
    }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> index;
    vector<int> lowlink;
    vector<bool> vis;
    stack<int> S;
    int depth;
    vector<vector<int>> components;

    void dfs(int u) {
        index[u] = depth;
        lowlink[u] = depth;
        depth++;
        S.push(u);
        vis[u] = true;

        for (int v : adj[u]) {
            if (!index[v]) {
                dfs(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (vis[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            vector<int> component;
            int v;
            do {
                v = S.top();
                S.pop();
                vis[v] = false;
                component.push_back(v);
            } while (u != v);
            components.push_back(move(component));
        }
    }

    int compute() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        vis.assign(V, false);
        S = stack<int>();
        depth = 1;
        components.clear();

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                dfs(u);
            }
        }
        return components.size();
    }
};
