#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include "graph_generator.hpp"

// *****

enum distance_graph_kind : int {
    DG_UNIFORM_SPARSE = 0,
    DG_UNIFORM_MODERATE = 1,
    DG_UNIFORM_DENSE = 2,
    DG_COMPLETE = 3,
    DG_MULTIPARTITE = 4,
    DG_DISJOINT_MULTIPARTITE = 5,
    DG_REGULAR_RING = 6,
    DG_NORMAL_GRID2 = 7,
    DG_CIRCULAR_GRID2 = 8,
    DG_NORMAL_GRID3 = 9,
    DG_CIRCULAR_GRID3 = 10,
    DG_END = 11,
};

string distance_kind_name[] = {
    "random uniform sparse", "random uniform moderate",
    "random uniform dense",  "complete",
    "multipartite",          "disjoint multipartite",
    "regular ring",          "normal 2d grid",
    "circular 2d grid",      "normal 3d grid",
    "circular 3d grid",      "---",
};

struct distance_graph {
    edges_t g;
    distance_graph_kind kind;
    int V, E, S, s, t;
    int X, Y, Z, n, k;
    vector<int> ints;
    double p;
    vector<long> weight;
};

// S arbitrates network size/complexity.
auto generate_distance_graph(distance_graph_kind i, int S, bool bidirectional = true) {
    assert(50 <= S && S <= 100'000);

    static auto rs = [](int x, double e) -> int { return ceil(pow(x, e)); };
    static auto dV = [](int x, double e) -> int {
        int n = max(3, rs(x, e)), s = sqrt(n);
        return intd(n, n + s)(mt);
    };
    static auto roughly = [](int x, double lo, double hi, int min, int max) -> int {
        return clamp(intd(rs(x, lo), rs(x, hi))(mt), min, max);
    };
    static auto bid = [](bool dir, edges_t& g) { !dir || (join(g, reverse(g)), true); };

    edges_t g;
    int V, X = 0, Y = 0, Z = 0, n = 0, k = 0;
    double p = 0.0;
    vector<int> ints;

    switch (i) {
    case DG_UNIFORM_SPARSE:
        V = dV(S, 0.88), p = reald(2.5, 3.5)(mt) / V;
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_MODERATE:
        V = dV(S, 0.78), p = reald(5.0, 7.5)(mt) / sqrt(V);
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_DENSE:
        V = dV(S, 0.76), p = reald(0.50, 0.95)(mt);
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_COMPLETE:
        V = dV(S, 0.75);
        g = complete_directed(V), bid(bidirectional, g);
        break;
    case DG_MULTIPARTITE:
        V = dV(S, 0.75), k = dV(S, 0.30), ints = partition_sample(V, k);
        g = complete_multipartite(ints), bid(bidirectional, g);
        break;
    case DG_DISJOINT_MULTIPARTITE:
        n = dV(S, 0.53), k = dV(S, 0.30), V = n * k;
        g = disjoint_complete_directed(n, k), bid(bidirectional, g);
        break;
    case DG_REGULAR_RING:
        V = dV(S, 0.80), k = roughly(S, 0.40, 0.55, 3, V - 1) & ~1;
        g = regular_ring(V, k), bid(bidirectional, g);
        break;
    case DG_NORMAL_GRID2:
        X = dV(S, 0.44), Y = dV(S, 0.44), V = X * Y;
        g = grid_directed(X, Y), bid(bidirectional, g);
        break;
    case DG_CIRCULAR_GRID2:
        X = dV(S, 0.44), Y = dV(S, 0.44), V = X * Y;
        g = circular_grid_directed(X, Y), bid(bidirectional, g);
        break;
    case DG_NORMAL_GRID3:
        X = dV(S, 0.28), Y = dV(S, 0.28), Z = dV(S, 0.28), V = X * Y * Z;
        g = grid3_directed(X, Y, Z), bid(bidirectional, g);
        break;
    case DG_CIRCULAR_GRID3:
        X = dV(S, 0.28), Y = dV(S, 0.28), Z = dV(S, 0.28), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z), bid(bidirectional, g);
        break;
    default:
        throw runtime_error("Invalid distance graph kind");
    }
    assert(verify_edges_directed(g, V));

    int E = g.size();
    distance_graph dg;
    dg.g = move(g);
    dg.kind = i;
    dg.V = V, dg.E = E, dg.S = S, dg.s = 0, dg.t = V - 1;
    dg.X = X, dg.Y = Y, dg.Z = Z;
    dg.ints = move(ints), dg.n = n, dg.k = k, dg.p = p;
    return dg;
}

auto add_weight_distance_graph(distance_graph& dg, long minweight, long maxweight) {
    dg.weight = int_gen<long>(dg.E, minweight, maxweight);
}
auto add_weight_distance_graph(distance_graph& dg, long maxweight) {
    return add_weight_distance_graph(dg, 1, maxweight);
}

#endif // DISTANCE_HPP
