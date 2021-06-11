#pragma once

#include "graph_generator.hpp"

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

struct bipartite_graph {
    edges_t g;
    int U, V, E, S;
    double p;
    vector<long> cost;
};

// S arbitrates network size/complexity.
auto generate_bipartite_graph(bipartite_graph_kind i, int S) {
    assert(50 <= S && S <= 100'000);

    static auto rs = [](int x, double e) -> int { return ceil(pow(x, e)); };
    static auto dV = [](int x, double e) -> int {
        int n = max(3, rs(x, e)), s = sqrt(n);
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int U = -1, V = -1;
    double p = 0.0;
    vector<int> ints;

    auto swapUV = [&]() { boold(0.5)(mt) ? swap(U, V) : (void)0; };

    switch (i) {
    case BG_UNIFORM_SPARSE:
        U = dV(S, 0.82), V = dV(S, 0.82), p = reald(2.5, 3.5)(mt) / V;
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE:
        U = dV(S, 0.75), V = dV(S, 0.75), p = reald(5.0, 6.0)(mt) / sqrt(V);
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE:
        U = dV(S, 0.68), V = dV(S, 0.68), p = reald(0.50, 0.95)(mt);
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_SPARSE_IMBALANCED:
        U = dV(S, 0.66), V = dV(S, 0.84), p = reald(2.5, 3.5)(mt) / V, swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_MODERATE_IMBALANCED:
        U = dV(S, 0.52), V = dV(S, 0.75), p = reald(5.0, 6.0)(mt) / sqrt(V), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_UNIFORM_DENSE_IMBALANCED:
        U = dV(S, 0.43), V = dV(S, 0.68), p = reald(0.50, 0.95)(mt), swapUV();
        g = random_uniform_bipartite(U, V, p);
        break;
    case BG_COMPLETE:
        U = dV(S, 0.68), V = dV(S, 0.68);
        g = complete_bipartite(U, V);
        break;
    case BG_COMPLETE_IMBALANCED:
        U = dV(S, 0.50), V = dV(S, 0.76);
        g = complete_bipartite(U, V);
        break;
    default:
        throw runtime_error("Invalid bipartite graph kind");
    }

    int E = g.size();
    bipartite_graph bg;
    bg.g = move(g);
    bg.U = U, bg.V = V, bg.E = E, bg.S = S;
    bg.p = p;
    return bg;
}

auto add_cost_bipartite_graph(bipartite_graph& bg, long mincost, long maxcost) {
    bg.cost = int_gen<long>(bg.E, mincost, maxcost);
}
auto add_cost_bipartite_graph(bipartite_graph& bg, long maxcost) {
    return add_cost_bipartite_graph(bg, 1, maxcost);
}
