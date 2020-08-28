#ifndef ISOMORPHISM_HPP
#define ISOMORPHISM_HPP

#include "graph.hpp"
#include "graph_operations.hpp"
#include "hash.hpp"

// *****

/**
 * Decent O(VE) heuristic for graph isomorphism. Came up with this for a codejam problem.
 * Two isomorphic graphs are always identified as such (no false-negatives).
 * However, non-isomorphic ones might be undistinguishable for this heuristic and generate
 * a false-positive.
 *
 * Includes also a fast topological graph hash which can be used for hashing graphs
 * upfront for isomorphism testing or vertex labeling.
 */

#define ALL(x) begin(x), end(x)

using hashtable_t = vector<size_t>;
using hashset_t = unordered_set<size_t>;
using bfs_t = vector<unordered_map<int, int>>;
using cnt_t = vector<int>;

void build_bfs_cnt(const graph& g, int n, bfs_t& bfs, cnt_t& cnt) {
    vector<bool> seen(g.V, false);
    vector<int> curr{n}, next;
    seen[n] = true;
    bfs = {{{n, 1}}};
    cnt = {1};
    int i = 0;
    while (!curr.empty()) {
        bfs.push_back({});
        cnt.push_back(0);
        for (int u : curr) {
            for (int v : g.adj[u]) {
                bfs[i + 1][v]++;
                cnt[i + 1]++;
            }
        }
        i++;
        for (int u : curr) {
            for (int v : g.adj[u]) {
                if (!seen[v]) {
                    seen[v] = true;
                    next.push_back(v);
                }
            }
        }
        curr = move(next);
    }
    bfs.pop_back();
    cnt.pop_back();
}

size_t build_hash(const bfs_t& bfs, const hashtable_t& ht) {
    vec_hasher hasher;
    pair_hasher phasher;
    int B = bfs.size();
    vector<size_t> hashes(B);
    for (int i = 0; i < B; i++) {
        int S = bfs[i].size();
        vector<size_t> layer(S);
        int j = 0;
        for (auto [u, c] : bfs[i]) {
            layer[j++] = phasher(pair<int, size_t>(c, ht[u]));
        }
        sort(begin(layer), end(layer));
        hashes[i] = hasher(layer);
    }
    return hasher(hashes);
}

/**
 * Compute the topological vertex hashes of a graph, irrespective of their labels.
 * Complexity: O(V^2 x iterations)
 * Useful for example to:
 *   - generate graphs that have only the trivial automorphism, in other words, the
 *     graph is different as seen from any of its vertices.
 *   - generate hashes of graphs to allow quick and dirty isomorphism test with
 *     low probability of false positives and no false negatives.
 */
hashtable_t hash_graph_vertices(const graph& g, int iterations = 3) {
    int V = g.V;
    hashtable_t hashtable(V);
    hashset_t hashset;
    vector<bfs_t> bfs(V);
    vec_hasher hasher;

    // step 1: build bfs and counting hash
    for (int n = 0; n < V; n++) {
        cnt_t cnt;
        build_bfs_cnt(g, n, bfs[n], cnt);
        auto h = hasher(cnt);
        hashtable[n] = h;
        hashset.insert(h);
    }

    // step 2+: recursive hash
    for (int i = 1; hashset.size() < uint(V) && i <= iterations; i++) {
        hashtable_t prev_hashtable(V);
        swap(hashtable, prev_hashtable);
        auto k = hashset.size();
        hashset.clear();

        for (int n = 0; n < V; n++) {
            auto h = build_hash(bfs[n], prev_hashtable);
            hashtable[n] = h;
            hashset.insert(h);
        }

        if (hashset.size() == k)
            break;
    }

    return hashtable;
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
 * Isomorphism heuristic from scratch
 * Complexity: O(1) or O(V^2)
 */
bool isomorphic(const graph& g1, const graph& g2) {
    return g1.V == g2.V && g1.E == g2.E && hash_graph(g1) == hash_graph(g2);
}

#endif // ISOMORPHISM_HPP
