#include "../graphs/topology.hpp"

#include "../generators/graph_operations.hpp"
#include "test_utils.hpp"

// *****

void unit_test_scc() {
    // vertex 0 is completely disconnected
    edges_t g = {{1, 2}, {2, 3}, {3, 1}, {4, 2}, {4, 3}, {4, 6}, {5, 3},
                 {5, 7}, {6, 4}, {6, 5}, {7, 5}, {8, 6}, {8, 7}, {8, 8}};

    strongly_connected_components scc;
    scc.V = 9, scc.out = make_adjacency_lists_directed(g, scc.V);

    scc.find_scc();
    scc.condensate();
    print("components #1: {}\n", scc.C);

    assert(scc.C == 5); // num components
    for (int c = 0; c < scc.C; c++) {
        sort(begin(scc.cset[c]), end(scc.cset[c]));
        sort(begin(scc.cadjout[c]), end(scc.cadjout[c]));
        sort(begin(scc.cadjin[c]), end(scc.cadjin[c]));
    }

    using vi = vector<int>;

    assert(scc.cset[0] == vi({0}));
    assert(scc.cset[1] == vi({1, 2, 3}));
    assert(scc.cset[2] == vi({5, 7}));
    assert(scc.cset[3] == vi({4, 6}));
    assert(scc.cset[4] == vi({8}));

    assert(scc.cadjout[0] == vi() && scc.cadjin[0] == vi());
    assert(scc.cadjout[1] == vi() && scc.cadjin[1] == vi({2, 3}));
    assert(scc.cadjout[2] == vi({1}) && scc.cadjin[2] == vi({3, 4}));
    assert(scc.cadjout[3] == vi({1, 2}) && scc.cadjin[3] == vi({4}));
    assert(scc.cadjout[4] == vi({2, 3}) && scc.cadjin[4] == vi());
}

void unit_test_dominator_tree() {
    dominator_tree dt;
    edges_t g;
    int V;
    enum nodenames { _, R = 1, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q };
    char name[] = "_RABCDEFGHIJKLMNOPQ";

    auto run = [&]() {
        dt.V = V, dt.out.assign(V + 1, {}), dt.in.assign(V + 1, {});
        for (auto [u, v] : g)
            dt.out[u].push_back(v), dt.in[v].push_back(u);
        dt.build_dominator_tree(R);
        for (int u = 1; u <= V; u++) {
            print("dom[{}] = {}  parent[{}] = {}\n", name[u], name[dt.dom[u]], name[u],
                  name[dt.parent[u]]);
        }
        print("---\n");
    };

    V = 13;
    g = {
        {R, A}, {R, B}, {R, C}, {A, D}, {B, A}, {B, D}, {B, E},
        {C, F}, {C, G}, {D, L}, {E, H}, {F, I}, {G, I}, {G, J},
        {H, E}, {H, K}, {I, K}, {J, I}, {K, I}, {K, R}, {L, H},
    };
    run();

    V = 17;
    g = {
        {R, A}, {R, I}, {A, B}, {A, C}, {A, H}, {I, B}, {J, A}, {B, C},
        {H, J}, {H, C}, {C, D}, {C, K}, {C, I}, {K, L}, {D, M}, {D, E},
        {D, H}, {L, D}, {L, K}, {L, E}, {L, P}, {M, D}, {M, N}, {E, F},
        {N, E}, {N, O}, {F, N}, {F, L}, {F, P}, {F, G}, {P, G}, {G, O},
    };
    run();
}

int main() {
    RUN_SHORT(unit_test_scc());
    RUN_SHORT(unit_test_dominator_tree());
    return 0;
}
