#ifndef GEN_DISTANCE_HPP
#define GEN_DISTANCE_HPP

#include "../graph_generator.hpp"

// *****

struct distance_graph {
    edges_t g;
    int V, E, S;
    costs_t weight;
};

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

// S arbitrates network size/complexity.
auto generate_distance_graph(distance_graph_kind i, int S, bool bidirectional = true) {
    assert(10 <= S && S <= 50000);

    reald sparsed(5.0, 12.0);
    reald moderated(4.0, 9.0);
    reald densed(0.6, 0.9);

    auto dV = [&](double e) {
        int n = max(3, int(ceil(pow(S, e)))), s = int(sqrt(n));
        return intd(n, n + s)(mt);
    };

    edges_t g;
    int V = -1, X, Y, Z, k, n;
    double p;
    offsets_t ints;

    auto sparse = [&]() { return min(1.0, sparsed(mt) / V); };
    auto moderate = [&]() { return min(1.0, moderated(mt) / sqrt(V)); };
    auto dense = [&]() { return densed(mt); };
    auto add_reverse = [&]() { bidirectional ? add_reverse_edges(g) : (void)0; };
    auto sqrtk = [&]() { return int(sqrt(V) + 1); };
    auto regk = [&]() { return max(2, (int(sqrt(V) + 1) | 1) - 1); };

    switch (i) {
    case DG_UNIFORM_SPARSE:
        V = dV(0.90), p = sparse();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_MODERATE:
        V = dV(0.80), p = moderate();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_UNIFORM_DENSE:
        V = dV(0.72), p = dense();
        g = random_uniform_directed_connected(V, p);
        break;
    case DG_COMPLETE:
        V = dV(0.68);
        g = complete_directed(V), add_reverse();
        break;
    case DG_MULTIPARTITE:
        V = dV(0.68), k = sqrtk(), ints = partition_sample(V, k);
        g = complete_multipartite(ints), add_reverse();
        break;
    case DG_DISJOINT_MULTIPARTITE:
        n = dV(0.75), k = dV(0.30), V = n * k;
        g = disjoint_complete_directed(n, k), add_reverse();
        break;
    case DG_REGULAR_RING:
        V = dV(0.85), k = regk();
        g = regular_ring(V, k), add_reverse();
        break;
    case DG_NORMAL_GRID2:
        X = dV(0.45), Y = dV(0.45), V = X * Y;
        g = grid_directed(X, Y), add_reverse();
        break;
    case DG_CIRCULAR_GRID2:
        X = dV(0.45), Y = dV(0.45), V = X * Y;
        g = circular_grid_directed(X, Y), add_reverse();
        break;
    case DG_NORMAL_GRID3:
        X = dV(0.27), Y = dV(0.27), Z = dV(0.27), V = X * Y * Z;
        g = grid3_directed(X, Y, Z), add_reverse();
        break;
    case DG_CIRCULAR_GRID3:
        X = dV(0.27), Y = dV(0.27), Z = dV(0.27), V = X * Y * Z;
        g = circular_grid3_directed(X, Y, Z), add_reverse();
        break;
    default:
        throw runtime_error("Invalid distance graph kind");
    }
    assert(verify_edges_directed(g, V));

    int E = g.size();
    distance_graph dg;
    dg.g = move(g);
    dg.V = V, dg.E = E, dg.S = S;
    return dg;
}

auto add_weight_distance_graph(distance_graph& dg, cost_t minweight, cost_t maxweight) {
    dg.weight = int_gen<cost_t>(dg.E, minweight, maxweight);
}
auto add_weight_distance_graph(distance_graph& dg, cost_t maxweight) {
    return add_weight_distance_graph(dg, 1, maxweight);
}

#endif // GEN_DISTANCE_HPP
