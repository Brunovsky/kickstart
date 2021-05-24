#include "test_utils.hpp"
#include "../graphs/topology.hpp"
#include "../graphs/dominator_tree.hpp"
#include "../graphs/scc.hpp"
#include "../graphs/euler_tour.hpp"
#include "../graphs/centroid_decomposition.hpp"
#include "../lib/graph_operations.hpp"
#include "../lib/graph_formats.hpp"
#include "../lib/graph_generator.hpp"

using vi = vector<int>;

inline namespace detail {

edges_t scan_edges(const string& s, char sep = ',') {
    edges_t g;
    stringstream ss(s);
    while (ss) {
        int u, v;
        char dummy;
        ss >> u >> dummy >> v;
        if (ss) {
            g.push_back({u, v});
            assert(dummy == sep);
        }
    }
    return g;
}

} // namespace detail

inline namespace unit_testing_builders {

void unit_test_centroid_decomposition() {
    int V = 38;
    string sedges = "0,1 0,2 0,3 0,4 1,5 1,6 2,7 2,8 3,9 3,10 6,11 6,36 6,37 7,28 7,29 "
                    "8,12 8,32 8,33 9,14 9,15 9,20 10,13 10,16 10,17 11,34 11,35 12,30 "
                    "12,31 13,18 13,19 14,24 14,25 14,26 14,27 15,21 15,23 21,22";

    edges_t g = scan_edges(sedges);
    print("g: {}\n", g);
    assert(int(g.size()) == V - 1);

    auto tree = make_adjacency_lists_undirected(V, g);
    auto decomp = build_tree_centroid_decomposition(tree);
    print("{}\n", decomp);
    for (int u = 0; u < V; u++) {
        print("cparent[{:2}] = {}\n", u, decomp[u]);
    }
}

void unit_test_scc() {
    // vertex 0 is completely disconnected
    int V = 9;
    edges_t g = {{1, 2}, {2, 3}, {3, 1}, {4, 2}, {4, 3}, {4, 6}, {5, 3},
                 {5, 7}, {6, 4}, {6, 5}, {7, 5}, {8, 6}, {8, 7}, {8, 8}};

    auto adj = make_adjacency_lists_directed(V, g);
    auto [cset, cmap] = build_scc(adj);
    auto [sccout, sccin] = condensate_scc(adj, cmap);
    int C = cset.size();

    print("components #1: {}\n", C);

    assert(C == 5); // num components
    for (int c = 0; c < C; c++) {
        sort(begin(cset[c]), end(cset[c]));
        sort(begin(sccout[c]), end(sccout[c]));
        sort(begin(sccin[c]), end(sccin[c]));
    }

    using vi = vector<int>;

    assert(cset[0] == vi({0}));
    assert(cset[1] == vi({1, 2, 3}));
    assert(cset[2] == vi({5, 7}));
    assert(cset[3] == vi({4, 6}));
    assert(cset[4] == vi({8}));

    assert(sccout[0] == vi() && sccin[0] == vi());
    assert(sccout[1] == vi() && sccin[1] == vi({2, 3}));
    assert(sccout[2] == vi({1}) && sccin[2] == vi({3, 4}));
    assert(sccout[3] == vi({1, 2}) && sccin[3] == vi({4}));
    assert(sccout[4] == vi({2, 3}) && sccin[4] == vi());
}

void unit_test_dominator_tree() {
    edges_t g;
    int V;
    enum nodenames { _, R = 1, A, B, C, D, E, F, G, H, I = 10, J, K, L, M, N, O, P, Q };
    char name[] = "_RABCDEFGHIJKLMNOPQ";

    auto run = [&]() {
        auto out = make_adjacency_lists_directed(V, g);
        auto [dom, parent] = build_dominator_tree(R, out);
        for (int u = 1; u < V; u++) {
            print("dom[{}] = {}  parent[{}] = {}\n", name[u], name[dom[u]], name[u],
                  name[parent[u]]);
        }
        print("---\n");
    };

    V = 14;
    g = {
        {R, A}, {R, B}, {R, C}, {A, D}, {B, A}, {B, D}, {B, E},
        {C, F}, {C, G}, {D, L}, {E, H}, {F, I}, {G, I}, {G, J},
        {H, E}, {H, K}, {I, K}, {J, I}, {K, I}, {K, R}, {L, H},
    };
    run();

    V = 18;
    g = {
        {R, A}, {R, I}, {A, B}, {A, C}, {A, H}, {I, B}, {J, A}, {B, C},
        {H, J}, {H, C}, {C, D}, {C, K}, {C, I}, {K, L}, {D, M}, {D, E},
        {D, H}, {L, D}, {L, K}, {L, E}, {L, P}, {M, D}, {M, N}, {E, F},
        {N, E}, {N, O}, {F, N}, {F, L}, {F, P}, {F, G}, {P, G}, {G, O},
    };
    run();
}

void unit_test_directed_euler_tour(int V = 8, int k = 4) {
    auto g = random_regular_directed_connected(V, k);
    int E = g.size();
    assert(E == V * k);

    auto path = build_directed_euler_tour(0, g);
    assert(int(path.size()) == E);

    vector<bool> seen(E, false);

    for (int i = 0; i < E; i++) {
        int e = path[i];
        if (i > 0) {
            int f = path[i - 1];
            assert(g[f][1] == g[e][0]); // edges link
        }
        assert(!seen[e]);
        seen[e] = true;
    }
}

void unit_test_undirected_euler_tour(int V = 8, int k = 4) {
    auto g = random_regular_connected(V, k);
    int E = g.size();
    assert(E == V * k / 2);

    auto path = build_undirected_euler_tour(0, g);
    assert(int(path.size()) == E);

    [[maybe_unused]] auto xore = [&](int e) { return g[e][0] ^ g[e][1]; };
    vector<bool> seen(E, false);

    for (int i = 0; i < E; i++) {
        int e = path[i];
        auto [v0, v1] = g[e];
        if (i > 0) {
            int f = path[i - 1];
            auto [u0, u1] = g[f];
            assert(u0 == v0 || u0 == v1 || u1 == v0 || u1 == v1);
            // ...u0 -> u1==v0 -> v1...
        }
        assert(!seen[e]);
        seen[e] = true;
    }
}

} // namespace unit_testing_builders

inline namespace unit_testing_finders {

void unit_test_tree_centers() {
    int V;
    vector<array<int, 2>> g;
    vector<vector<int>> tree;
    vector<int> centers, diameter;

    V = 14;
    g = {
        {0, 2},   {1, 2},  {2, 5}, {3, 4}, {4, 5}, {11, 10}, {12, 10},
        {13, 10}, {10, 8}, {8, 9}, {6, 9}, {9, 7}, {5, 9},
    };
    tree = make_adjacency_lists_undirected(V, g);
    centers = find_tree_centers(tree);
    diameter = find_tree_diameter(tree);
    print("centers: {}\n", centers);
    print("diameter: {}\n", diameter);
    assert(centers.size() == 1 && centers[0] == 9);

    V = 11;
    g = {{0, 2}, {1, 2}, {2, 3}, {3, 4}, {3, 5}, {5, 6}, {5, 7}, {5, 8}, {8, 10}, {8, 9}};
    tree = make_adjacency_lists_undirected(V, g);
    centers = find_tree_centers(tree);
    diameter = find_tree_diameter(tree);
    sort(begin(centers), end(centers));
    print("centers: {}\n", centers);
    print("diameter: {}\n", diameter);
    assert(centers.size() == 2 && centers == vi({3, 5}));
}

} // namespace unit_testing_finders

int main() {
    RUN_SHORT(unit_test_centroid_decomposition());
    RUN_SHORT(unit_test_scc());
    RUN_SHORT(unit_test_dominator_tree());
    RUN_SHORT(unit_test_tree_centers());
    RUN_SHORT(unit_test_directed_euler_tour(8, 4));
    RUN_SHORT(unit_test_directed_euler_tour(20, 6));
    RUN_SHORT(unit_test_directed_euler_tour(200, 20));
    RUN_SHORT(unit_test_undirected_euler_tour(8, 4));
    RUN_SHORT(unit_test_undirected_euler_tour(20, 6));
    RUN_SHORT(unit_test_undirected_euler_tour(200, 20));
    return 0;
}
