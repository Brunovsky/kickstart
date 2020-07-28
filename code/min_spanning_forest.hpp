#include <bits/stdc++.h>

#include "disjoint_set.hpp"

using namespace std;

// *****

struct min_spanning_forest {
    int V, E;
    vector<vector<int>> adj;
    vector<int> source;
    vector<int> target;
    vector<int> weight;

    min_spanning_forest(int V) : V(V), E(0) {
        adj.resize(V, {});
    }

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        int e = E++;
        source.resize(E), target.resize(E), weight.resize(E);
        source[e] = u;
        target[e] = v;
        weight[e] = w;
        adj[u].push_back(e);
    }

    int compute() {
        disjoint_set set(V);
        vector<int> edges(E);
        iota(begin(edges), end(edges), 0);
        sort(begin(edges), end(edges), [](int e1, int e2) {
            return weight[e1] < weight[e2];
        });

        int total_weight = 0;
        for (int e : edges) {
            if (set.find(source[e]) != set.find(target[e])) {
                total_weight += weight[e];
                set.join(source[e], target[e]);
            }
        }
        return total_weight;
    }
};
