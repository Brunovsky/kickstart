#include "test_utils.hpp"
#include "../lib/graph_operations.hpp"
#include "../struct/lca.hpp"

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

inline namespace unit_testing_lca {

template <typename LCA>
void unit_test_lca_tree() {
    int V = 20;
    string s = "1,2 1,3 1,4 1,5 2,6 2,7 3,8 3,9 3,10 5,11 5,12 5,13 "
               "7,14 10,15 10,16 13,17 13,18 13,19";
    auto g = scan_edges(s);
    random_flip_inplace(g);
    auto tree = make_adjacency_lists_undirected(V, g);

    LCA lca(tree, 1);

    assert(lca.lca(11, 19) == 5);
    assert(lca.lca(9, 15) == 3);
    assert(lca.lca(14, 15) == 1);
    assert(lca.lca(11, 13) == 5);
    assert(lca.depth[8] == 2);
    assert(lca.depth[16] == 3);
    assert(lca.dist(7, 17) == 5);
    assert(lca.dist(6, 8) == 4);
    assert(lca.dist(3, 3) == 0);
    assert(lca.dist(3, 15) == 2);
}

} // namespace unit_testing_lca

int main() {
    RUN_SHORT(unit_test_lca_tree<lca_binary>());
    RUN_SHORT(unit_test_lca_tree<lca_rmq>());
    RUN_SHORT(unit_test_lca_tree<lca_schieber_vishkin>());
    return 0;
}
