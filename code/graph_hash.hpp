#include "graph.hpp"

// *****

/**
 * Compute the topological vertex hashes of a graph, irrespective of their labels.
 * Complexity: O(V^2 x iterations x threshold)
 */
vector<size_t> hash_graph_vertices(const graph& g) {
    constexpr int hash_iterations = 3;
    constexpr int bfs_threshold = 10; // 10 * V edges
    vec_hasher hasher;

    int V = g.V;
    vector<size_t> hashtable[2];
    hashtable[0].resize(V);
    hashtable[1].resize(V);

    vector<vector<vector<int>>> bfs(V);

    for (int w = 0; w < V; w++) {
        vector<bool> seen(V, false);
        seen[w] = true;
        bfs[w].push_back({w});
        int S = 1, i = 0;
        int total = 1;

        while (i++ < V && S < V && total < bfs_threshold * V) {
            bfs[w].push_back({});
            for (int u : bfs[w][i - 1]) {
                for (int v : g.adj[u]) {
                    if (!seen[v]) {
                        seen[v] = true;
                        bfs[w][i].push_back(v);
                    }
                }
                total += g.adj[u].size();
            }
            for (int v : bfs[w][i]) {
                seen[v] = false;
            }
            S = bfs[w][i].size();
        }
    }

    // round 0: counting hash
    for (int w = 0; w < V; w++) {
        int B = bfs[w].size();
        vector<int> cnts(B);
        for (int i = 0; i < B; i++) {
            cnts[i] = bfs[w][i].size();
        }
        hashtable[0][w] = hasher(cnts);
    }

    // round 1+: recursive hash
    for (int iteration = 1; iteration <= hash_iterations; iteration++) {
        swap(hashtable[0], hashtable[1]);
        for (int w = 0; w < V; w++) {
            int B = bfs[w].size();
            vector<size_t> hashes(B);
            for (int i = 0; i < B; i++) {
                int S = bfs[w][i].size();
                vector<size_t> layer(S);
                for (int j = 0; j < S; j++) {
                    layer[j] = hashtable[1][bfs[w][i][j]];
                }
                sort(begin(layer), end(layer));
                hashes[i] = hasher(layer);
            }
            hashtable[0][w] = hasher(hashes);
        }
    }

    return hashtable[0];
}

/**
 * Compute the topological hash of a graph, irrespective of its labels.
 * Complexity: O(V^2)
 */
size_t hash_graph(const graph& g) {
    auto hashtable = hash_graph_vertices(g);
    sort(begin(hashtable), end(hashtable));
    hashtable.push_back(g.V), hashtable.push_back(g.E);
    return vec_hasher{}(hashtable);
}

/**
 * Check with negligible false positive probability whether two graphs are isomorphic.
 * Precompute the hashes instead
 * Complexity: O(1) or O(V^2)
 */
bool isomorphic(graph& lhs, graph& rhs) {
    return lhs.V == rhs.V && lhs.E == rhs.E && hash_graph(lhs) == hash_graph(rhs);
}
