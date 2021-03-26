#ifndef SCC_HPP
#define SCC_HPP

#include "graph.hpp"

// *****

/**
 * Operations on the strongly connected components of a graph
 */
struct strongly_connected_components {
    int V, C = 0;                  // number of strongly connected components
    vector<vector<int>> adj, cset; // cset[c]: nodes in component c
    vector<int> cmap;              // cmap[u]: component of node u

    strongly_connected_components(int V, const edges_t& g) : V(V), adj(V), cmap(V) {
        for (auto [u, v] : g)
            adj[u].push_back(v);
    }

    vector<int> index, lowlink;
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
                v = S.top(), S.pop();
                onstack[v] = false;
                cset[c].push_back(v);
                cmap[v] = c;
            } while (u != v);
        }
    }

    int compute() {
        index.assign(V, 0);
        lowlink.assign(V, 0);
        onstack.assign(V, false);
        S = stack<int>();
        depth = 1;
        cset.clear();

        for (int u = 0; u < V; u++) {
            if (!index[u]) {
                tarjan(u);
            }
        }
        return C = cset.size();
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

#endif // SCC_HPP
