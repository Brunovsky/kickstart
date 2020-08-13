#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Operations on the strongly connected components of a graph
 */
struct strongly_connected_components {
    int V, C;
    vector<vector<int>> adj;
    vector<vector<int>> cset;
    vector<int> cmap;

    strongly_connected_components(int V) : V(V) { adj.resize(V, {}); }

    void add(int u, int v) {
        assert(0 <= u && u < V && 0 <= v && v < V);
        adj[u].push_back(v);
    }

    vector<int> index;
    vector<int> lowlink;
    vector<bool> onstack;
    stack<int> S;
    int depth;

    void tarjan(int u) {
        index[u] = depth;
        lowlink[u] = depth;
        depth++;
        S.push(u);
        onstack[u] = true;

        for (int v : adj[u]) {
            if (!index[v]) {
                tarjan(v);
                lowlink[u] = min(lowlink[u], lowlink[v]);
            } else if (onstack[v]) {
                lowlink[u] = min(lowlink[u], index[v]);
            }
        }

        if (index[u] == lowlink[u]) {
            int c = cset.size();
            cset.push_back({});
            int v;
            do {
                v = S.top();
                S.pop();
                onstack[v] = false;
                cset[c].push_back(v);
                cmap[v] = c;
            } while (u != v);
        }
    }

    void compute() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        onstack.assign(V, false);
        S = stack<int>();
        depth = 1;
        cset.clear();
        cmap.assign(V, 0);

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                tarjan(u);
            }
        }
        C = cset.size();
    }

    // If necessary, build the condensated graph on scc
    // the components are enumerated in reverse topological order, so the topological
    // order of components is C-1..0
    vector<vector<int>> cadjout;
    vector<vector<int>> cadjin;

    void condensate() {
        cadjout.assign(C, {});
        cadjin.assign(C, {});
        vector<bool> seen(C, false);

        // use an unordered map etc if you need to quantify incidence between scc
        for (int c = 0; c < C; c++) {
            seen[c] = true;
            for (int u : cset[c]) {
                for (int v : adj[u]) {
                    int d = cmap[v];
                    if (!seen[d]) {
                        seen[d] = true;
                        cadjout[c].push_back(d);
                        cadjin[d].push_back(c);
                    }
                }
            }
            seen[c] = false;
            for (int d : cadjout[c])
                seen[d] = false;
        }
    }
};
