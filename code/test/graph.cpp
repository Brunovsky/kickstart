#include "../graph_formats.hpp"
#include "../graph_generator.hpp"

// *****

static int si = 1;

template <typename G>
void show(string msg, G&& g) {
    cout << string(70, '=') << "\n\n";
    cout << si++ << " # " << msg << "\n\n" << g << "\n";
    cout << string(70, '=') << endl;
}

void test_generator() {
    show("Path undirected 10", path_undirected(10));
    show("Path directed 10", path_directed(10));
    show("Grid undirected 4x5", grid_undirected(4, 5));
    show("Grid directed 4x5", grid_directed(4, 5));
    show("Grid circular undirected 4x5", circular_grid_undirected(4, 5));
    show("Grid circular directed 4x5", circular_grid_directed(4, 5));
    show("Grid3 undirected 3x4x3", grid3_undirected(3, 4, 3));
    show("Grid3 directed 3x4x3", grid3_directed(3, 4, 3));
    show("Grid3 circular undirected 3x4x3", circular_grid3_undirected(3, 4, 3));
    show("Grid3 circular directed 3x4x3", circular_grid3_directed(3, 4, 3));
    show("Complete undirected 6", complete_undirected(6));
    show("Complete directed 6", complete_directed(6));
    show("Cycle undirected 6", cycle_undirected(6));
    show("Cycle directed 6", cycle_directed(6));
    show("Regular ring 9,4", regular_ring(9, 4));
    show("Perfect binary tree height 5", perfect_binary_tree_undirected(5));
    show("Perfect binary tree height 5 directed", perfect_binary_tree_directed(5));
    show("Perfect 3-ary tree height 3", perfect_tree_undirected(3, 3));
    show("Multipartite graph 2,3,4,5", complete_multipartite({2, 3, 4, 5}));

    show("Johnson graph (7,2)", johnson(7, 2));
    show("Johnson graph (5,3)", johnson(5, 3));
    show("Kneser graph (8,3)", kneser(8, 3));
    show("Kneser graph (9,4)", kneser(9, 4));
    show("Wheel graph 10", wheel(10));
    show("Cogwheel graph 5", cogwheel(5));
    show("Web graph 6,3", web(6, 3));
    show("Turan graph 15,4", turan(15, 4));
    show("Circulant graph 14 (2,4,7)", circulant(14, {2, 4, 7}));
    show("Sudoku 9x9", sudoku(9));
    show("Sudoku 4x4", sudoku(4));

    show("Random tree undirected 8", random_tree_undirected(8));
    show("Random tree directed 8", random_tree_directed(8));
    show("Random regular 10,3", random_regular(10, 3));
    show("Random regular 10,3 connected", random_regular_connected(10, 3));

    show("Random uniform undirected 11,0.3", random_uniform_undirected(11, 0.3));
    show("Random uniform undirected 11,0.7", random_uniform_undirected(11, 0.7));
    show("Random uniform directed 9,0.2", random_uniform_directed(9, 0.2));
    show("Random uniform directed 9,0.8", random_uniform_directed(9, 0.8));
    show("Random exact undirected 12,26", random_exact_undirected(12, 26));
    show("Random exact directed 12,26", random_exact_directed(12, 26));

    show("Random uniform undirected connected 11,0.3",
         random_uniform_undirected_connected(11, 0.3));
    show("Random uniform undirected connected 11,0.7",
         random_uniform_undirected_connected(11, 0.7));
    show("Random uniform rooted dag connected 9,0.2",
         random_uniform_rooted_dag_connected(9, 0.2));
    show("Random uniform rooted dag connected 9,0.8",
         random_uniform_rooted_dag_connected(9, 0.8));
    show("Random exact undirected connected 12,26",
         random_exact_undirected_connected(12, 26));
    show("Random exact rooted dag connected 12,26",
         random_exact_rooted_dag_connected(12, 26));

    show("Disjoint K4 x5", disjoint_complete_undirected(5, 4));
    show("One-connected K4 x5", one_connected_complete_undirected(5, 4));
    show("Tri-connected K4 x5", k_connected_complete_undirected(5, 4));

    show("Random full level 5 ranks V=20", random_full_level(20, 5, 2));
    show("Random full level dag 5 ranks V=20", random_full_level_dag(20, 5, 2));
    show("Random full level flow 7 ranks V=20", random_full_level_flow(20, 7, 2));
    show("Random uniform level 5 ranks V=30", random_uniform_level(30, 0.4, 5, 2));
    show("Random uniform level dag 5 ranks V=30",
         random_uniform_level_dag(30, 0.4, 5, 2));
    show("Random uniform level flow 7 ranks V=30",
         random_uniform_level_flow(30, 0.4, 7, 2));
}

int main() {
    test_generator();
    return 0;
}
