#ifndef GEN_BIPARTITE_HPP
#define GEN_BIPARTITE_HPP

#include "../graph_generator.hpp"

// *****

struct bipartite_graph {
    edges_t g;
    int U, V, E, S;
    costs_t cost;
};

enum bipartite_graph_kind : int {
    BG_UNIFORM_SPARSE = 0,
    BG_UNIFORM_MODERATE = 1,
    BG_UNIFORM_DENSE = 2,
    BG_UNIFORM_SPARSE_IMBALANCED = 3,
    BG_UNIFORM_MODERATE_IMBALANCED = 4,
    BG_UNIFORM_DENSE_IMBALANCED = 5,
    BG_COMPLETE = 6,
    BG_COMPLETE_IMBALANCED = 7,
    BG_END = 8,
};

string bipartite_kind_name[] = {
    "random uniform sparse",
    "random uniform moderate",
    "random uniform dense",
    "random uniform sparse imbalanced",
    "random uniform moderate imbalanced",
    "random uniform dense imbalanced",
    "complete",
    "complete imbalanced",
    "regular",
};

// S arbitrates network size/complexity.
auto generate_bipartite_graph(bipartite_graph_kind i, int S) {
    assert(10 <= S && S <= 50000);

    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);
    boold swapd(0.5);

    auto dV = [&](double e) {
        int n = max(1, int(ceil(pow(S, e)))), s = int(sqrt(n));
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int U = -1, V = -1;
    double p;
    offsets_t ints;

    auto swapUV = [&]() { swapd(mt) ? swap(U, V) : (void)0; };
    auto sparse = [&]() { return min(1.0, sparsed(mt) / sqrt(U * V)); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(sqrt(U * V))); };
    auto dense = [&]() { return densed(mt); };

    switch (i) {
    case BG_UNIFORM_SPARSE:
        U = dV(0.82), V = dV(0.82), p = sparse();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE:
        U = dV(0.75), V = dV(0.75), p = moderate();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE:
        U = dV(0.68), V = dV(0.68), p = dense();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_SPARSE_IMBALANCED:
        U = dV(0.66), V = dV(0.84), p = sparse(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE_IMBALANCED:
        U = dV(0.52), V = dV(0.75), p = moderate(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE_IMBALANCED:
        U = dV(0.43), V = dV(0.68), p = dense(), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_COMPLETE:
        U = dV(0.68), V = dV(0.68);
        g = complete_bipartite(U, V);
        break;
    case BG_COMPLETE_IMBALANCED:
        U = dV(0.50), V = dV(0.76);
        g = complete_bipartite(U, V);
        break;
    default:
        throw runtime_error("Invalid bipartite graph kind");
    }

    int E = g.size();
    bipartite_graph bg;
    bg.g = move(g);
    bg.U = U, bg.V = V, bg.E = E, bg.S = S;
    return bg;
}

auto add_cost_bipartite_graph(bipartite_graph& bg, cost_t mincost, cost_t maxcost) {
    bg.cost = int_gen<cost_t>(bg.E, mincost, maxcost);
}
auto add_cost_bipartite_graph(bipartite_graph& bg, cost_t maxcost) {
    return add_cost_bipartite_graph(bg, 1, maxcost);
}

/**
 * To generate a bipartite graph (U,V) with maximum matching M, start with an empty
 * bipartite graph and add edges (n,n) for n=0...M-1. Then add more edges as follows:
 * - do not add any edge (u,v) where u>=M and v>=M
 * - for each n=0...M-1 pick a side:
 *     - if U side, add edges(n,v) where v>=M (there are V-M such possible edges),
 *     - if V side, add edges(u,n) where u>=M (there are U-M such possible edges).
 */
edges_t random_bipartite_maximum_matching(int U, int V, int M, double p,
                                          double uside = 0.5) {
    assert(M <= U && M <= V && 0.0 <= p && p <= 1.0 && 0.0 <= uside && uside <= 1.0);
    edges_t g;
    boold fixud(uside);
    binomd Ubinom(U - M, p), Vbinom(V - M, p);

    for (int n = 0; n < M; n++) {
        g.push_back({n, n});
        if (fixud(mt)) {
            if (M < V)
                for (int v : int_sample(Vbinom(mt), M, V - 1))
                    g.push_back({n, v});
        } else {
            if (M < U)
                for (int u : int_sample(Ubinom(mt), M, U - 1))
                    g.push_back({u, n});
        }
    }

    shuffle(begin(g), end(g), mt);
    relabel_inplace(U, V, g);
    return g;
}

#endif // GEN_BIPARTITE_HPP
