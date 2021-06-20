#pragma once

#include "graph_generator.hpp"

/**
 * Generate a random Erdos bipartite graph with maximum matching M.
 * This algorithm is not perfect; not all topologies can be generated, I think, and some
 * topologies will appear more frequently due to symmetry.
 *
 * To generate a bipartite graph on (U,V) vertices with maximum matching M.
 * First add edges (n,n) for n=0...M-1. This is the canonical matching.
 * Then add more edges as follows:
 * - Case 1: intend to add E-M more edges (to get E total):
 *   - Do not add any edge (u,v) where u>=M and v>=M
 *   - Pick some m in 0..M. We will add some of the following edges:
 *     - a: (u,v) where 0<=u<m and 0<=v<m. (if u=v it already exists)
 *     - b: (u,v) where m<=u<M and m<=v<M. (if u=v it already exists)
 *     - c: (u,v) where 0<=u<m and M<=v.
 *     - d: (u,v) where M<=u   and m<=v<M
 *   * - a: m^2 edges | b: (M-m)^2 edges | c: m(V-M) edges | d: (M-m)(U-M) edges
 *     - total: m^2 + m(V-U-M) + UM edges. must pick m such that this is >=E.
 *     - discriminant: D = (V-U-M)^2 + 4E -4UM
 *     - m<=lo=1/2(M+U-V-sqrt(D)) OR m>=hi=1/2(M+U-V+sqrt(D))
 *   - Pick a partition of E-M into 4 parts with the upper cap above (*).
 *   - Add the edges using pair_sample.
 * - Case 2. intend to add edges with probability p
 *   - Call a binomd to determine E and proceed with case 1.
 * Finally, relabel and shuffle to hide the topology.
 */
auto bipartite_matching_max_edges(int U, int V, int M) {
    assert(M <= U && M <= V);
    return 1L * M * max(U, V);
}

auto bipartite_matching_group_sizes(int U, int V, int M, int E) {
    assert(M <= U && M <= V && M <= E && E <= bipartite_matching_max_edges(U, V, M));
    long delta = 1L * (V - U - M) * (V - U - M) + 4L * E - 4L * U * M;
    int lo = (M + U - V - sqrt(delta)) / 2;
    int hi = (M + U - V + sqrt(delta) + 1) / 2;
    assert(0 <= lo || hi <= M);

    lo = clamp(lo, -1, M), hi = clamp(hi, lo + 1, M + 1);
    int m = intd(0, lo + M - hi + 1)(mt);
    m += (m > lo) * (hi - lo - 1);

    int A = min(1L * INT_MAX, 1L * m * m - m);                   // m already matched
    int B = min(1L * INT_MAX, 1L * (M - m) * (M - m) - (M - m)); // M - m already matched
    int C = min(1L * INT_MAX, 1L * m * (V - M));
    int D = min(1L * INT_MAX, 1L * (M - m) * (U - M));

    auto s = partition_sample_balanced(E - M, 4, {0, 0, 0, 0}, {A, B, C, D});
    int a = s[0], b = s[1], c = s[2], d = s[3];
    return array<int, 5>{m, a, b, c, d};
}

auto bipartite_matching_hide_topology(int U, int V, edges_t& g) {
    shuffle(begin(g), end(g), mt);
    random_relabel_graph_inplace(U, V, g);
}

auto random_bipartite_matching(int U, int V, int M, int E) {
    auto [m, a, b, c, d] = bipartite_matching_group_sizes(U, V, M, E);
    edges_t g;
    g.reserve(E);

    for (int n = 0; n < M; n++)
        g.push_back({n, n});
    for (auto [u, v] : distinct_pair_sample(a, 0, m))
        g.push_back({u, v});
    for (auto [u, v] : distinct_pair_sample(b, m, M))
        g.push_back({u, v});
    for (auto [u, v] : pair_sample(c, 0, m, M, V))
        g.push_back({u, v});
    for (auto [u, v] : pair_sample(d, M, U, m, M))
        g.push_back({u, v});

    return g;
}

auto random_bipartite_matching(int U, int V, int M, double p) {
    binomd distE(bipartite_matching_max_edges(U, V, M), clamp(p, 0.0, 1.0));
    int E = max(M, int(distE(mt)));
    return random_bipartite_matching(U, V, M, E);
}
