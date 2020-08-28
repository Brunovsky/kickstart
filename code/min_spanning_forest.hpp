#ifndef MIN_SPANNING_FOREST_HPP
#define MIN_SPANNING_FOREST_HPP

#include "disjoint_set.hpp"

// *****

/**
 * Kruskal's algorithm for minimum spanning forest
 * Complexity: O(E log E)
 */
struct min_spanning_forest {
    int V, E;
    vector<int> source;
    vector<int> target;
    vector<int> weight;

    min_spanning_forest(int V) : V(V), E(0) {}

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }

    int compute() {
        disjoint_set set(V);
        vector<int> edges(E);
        iota(begin(edges), end(edges), 0);
        sort(begin(edges), end(edges), [this](int e1, int e2) {
            return weight[e1] < weight[e2];
        });

        int msf_weight = 0;
        for (int e : edges) {
            int u = source[e], v = target[e];
            if (set.find(u) != set.find(v)) {
                msf_weight += weight[e];
                set.join(u, v);
                // add e to a vec if you want to output the tree's edges
            }
        }
        return msf_weight;
    }
};

#endif // MIN_SPANNING_FOREST_HPP
