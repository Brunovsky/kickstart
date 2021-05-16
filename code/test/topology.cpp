#include "test_utils.hpp"
#include "../graphs/topology.hpp"
#include "../graphs/dominator_tree.hpp"
#include "../graphs/scc.hpp"
#include "../lib/graph_operations.hpp"

inline namespace unit_testing {

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

} // namespace unit_testing

int main() {
    RUN_SHORT(unit_test_scc());
    RUN_SHORT(unit_test_dominator_tree());
    return 0;
}
