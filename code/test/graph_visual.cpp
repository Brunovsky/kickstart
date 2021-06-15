#include "test_utils.hpp"
#include "../lib/graph_generator.hpp"
#include "../lib/graph_formats.hpp"

inline namespace detail {

static int si = 1;

void showu(string msg, const edges_t& g, int V) {
    cout << string(70, '=') << "\n\n";
    cout << si++ << " # " << msg << "\n\n" << to_human_undirected(g, V) << "\n";
    cout << string(70, '=') << endl;
}
void showd(string msg, const edges_t& g, int V) {
    cout << string(70, '=') << "\n\n";
    cout << si++ << " # " << msg << "\n\n" << to_human_directed(g, V) << "\n";
    cout << string(70, '=') << endl;
}
void showb(string msg, const edges_t& g, int U, int V) {
    cout << string(70, '=') << "\n\n";
    cout << si++ << " # " << msg << "\n\n" << to_human_bipartite(g, U, V) << "\n";
    cout << string(70, '=') << endl;
}
void showu(string msg, const edges_t& g) {
    int V = 0;
    for (auto [u, v] : g)
        V = max(V, 1 + max(u, v));
    showu(msg, g, V);
}
void showd(string msg, const edges_t& g) {
    int V = 0;
    for (auto [u, v] : g)
        V = max(V, 1 + max(u, v));
    showd(msg, g, V);
}
void showb(string msg, const edges_t& g) {
    int U = 0, V = 0;
    for (auto [u, v] : g)
        U = max(U, u + 1), V = max(V, 1 + v);
    showb(msg, g, U, V);
}

} // namespace detail

void visual_test_generators() {
    showu("Path undirected 10", path_undirected(10));
    showd("Path directed 10", path_directed(10));
    showu("Complete undirected 6", complete_undirected(6));
    showd("Complete directed 6", complete_directed(6));
    showu("Cycle undirected 6", cycle_undirected(6));
    showd("Cycle directed 6", cycle_directed(6));
    showu("Regular ring 9,4", regular_ring(9, 4));
    showu("Perfect binary tree height 5", perfect_binary_tree_undirected(5));
    showd("Perfect binary tree height 5 directed", perfect_binary_tree_directed(5));
    showu("Perfect 3-ary tree height 3", perfect_tree_undirected(3, 3));
    showu("Multipartite graph 2,3,4,5", complete_multipartite({2, 3, 4, 5}));

    showu("Johnson graph (7,2)", johnson(7, 2));
    showu("Johnson graph (5,3)", johnson(5, 3));
    showu("Kneser graph (8,3)", kneser(8, 3));
    showu("Kneser graph (9,4)", kneser(9, 4));
    showu("Wheel graph 10", wheel(10));
    showu("Cogwheel graph 5", cogwheel(5));
    showu("Web graph 6,3", web(6, 3));
    showu("Turan graph 15,4", turan(15, 4));
    showu("Circulant graph 14 (2,4,7)", circulant(14, {2, 4, 7}));
    showu("Sudoku 9x9", sudoku(9));
    showu("Sudoku 4x4", sudoku(4));

    showu("Random tree undirected 8", random_tree_undirected(8));
    showd("Random tree directed 8", random_tree_directed(8));

    showu("Random uniform undirected 11,0.3", random_uniform_undirected(11, 0.3));
    showu("Random uniform undirected 11,0.7", random_uniform_undirected(11, 0.7));
    showd("Random uniform directed 9,0.2", random_uniform_directed(9, 0.2));
    showd("Random uniform directed 9,0.8", random_uniform_directed(9, 0.8));
    showu("Random exact undirected 12,26", random_exact_undirected(12, 26));
    showd("Random exact directed 12,26", random_exact_directed(12, 26));

    showu("Random uniform undirected connected 11,0.3",
          random_uniform_undirected_connected(11, 0.3));
    showu("Random uniform undirected connected 11,0.7",
          random_uniform_undirected_connected(11, 0.7));
    showd("Random uniform rooted dag connected 9,0.2",
          random_uniform_rooted_dag_connected(9, 0.2));
    showd("Random uniform rooted dag connected 9,0.8",
          random_uniform_rooted_dag_connected(9, 0.8));
    showd("Random uniform directed connected 10,0.2",
          random_uniform_directed_connected(10, 0.2));
    showd("Random uniform directed connected 10,0.8",
          random_uniform_directed_connected(10, 0.8));
    showu("Random exact undirected connected 12,26",
          random_exact_undirected_connected(12, 26));
    showd("Random exact rooted dag connected 12,26",
          random_exact_rooted_dag_connected(12, 26));
    showd("Random exact directed connected 12,26",
          random_exact_directed_connected(12, 26));

    showu("Random regular 15,6", random_regular(15, 6));
    showu("Random regular 16,4 connected", random_regular_connected(16, 4));
    showb("Random regular bipartite 10,25,5", random_regular_bipartite(10, 25, 5));

    showu("Disjoint K4 x5", disjoint_complete_undirected(5, 4));
    showu("One-connected K4 x5", one_connected_complete_undirected(5, 4));
    showu("Tri-connected K4 x5", k_connected_complete_undirected(5, 4));

    showu("Random full level 5 ranks V=20", random_full_level(20, 5, 2));
    showd("Random full level dag 5 ranks V=20", random_full_level_dag(20, 5, 2));
    showd("Random full level flow 7 ranks V=20", random_full_level_flow(20, 7, 2));
    showu("Random uniform level 5 ranks V=30", random_uniform_level(30, 0.4, 5, 2));
    showd("Random uniform level dag 5 ranks V=30",
          random_uniform_level_dag(30, 0.4, 5, 2));
    showd("Random uniform level flow 7 ranks V=30",
          random_uniform_level_flow(30, 0.4, 7, 2));

    showb("Random uniform bipartite 10,20,0.3", random_uniform_bipartite(10, 20, 0.3));
    showb("Random uniform bipartite 10,20,0.7", random_uniform_bipartite(10, 20, 0.7));
    showb("Random exact bipartite 10,20,73", random_exact_bipartite(10, 20, 73));
    showb("Random exact bipartite 10,25,43", random_exact_bipartite(10, 25, 43));

    showu("Grid undirected 6x5", grid_undirected(6, 5));
    showd("Grid directed 6x5", grid_directed(6, 5));
    showu("Grid circular undirected 6x5", circular_grid_undirected(6, 5));
    showd("Grid circular directed 6x5", circular_grid_directed(6, 5));
    showu("Grid3 undirected 3x4x3", grid3_undirected(3, 4, 3));
    showd("Grid3 directed 3x4x3", grid3_directed(3, 4, 3));
    showu("Grid3 circular undirected 3x4x3", circular_grid3_undirected(3, 4, 3));
    showd("Grid3 circular directed 3x4x3", circular_grid3_directed(3, 4, 3));
}

int main() {
    RUN_BLOCK(visual_test_generators());
    return 0;
}