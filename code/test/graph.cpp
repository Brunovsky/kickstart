#include "../debug_print.hpp"
#include "../graph_formats.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

static int si = 1;

template <typename G>
void show(string msg, G&& g) {
    cout << string(70, '=') << "\n\n";
    cout << si++ << " # " << msg << "\n\n" << g << "\n";
    cout << string(70, '=') << endl;
}

void random_regular_performance(int R, int n, int k) {
    if (R == 0)
        return;

    auto now = steady_clock::now();
    for (int i = 1; i <= R; i++) {
        print("\rrandom regular {}...", i);
        auto g = random_regular(n, k);
    }
    auto time = duration_cast<ms>(steady_clock::now() - now);

    long E = 1L * R * (n * k / 2);
    long T = time.count();
    double per = 1e6 * T / E;

    print("\r{}", string(50, ' '));
    print("\r{:>6} {:>6} {:>4} {:>9} | {:5}ms  {:<6.1f}ms per 1M\n", R, n, k, E, T, per);
}

void test_random_regular_performance(double M = 1) {
    random_regular_performance(M * 10, 300, 100);
    random_regular_performance(M * 10, 300, 150);
    random_regular_performance(M * 10, 300, 200);
    random_regular_performance(M * 5000, 25, 4);
    random_regular_performance(M * 5000, 25, 6);
    random_regular_performance(M * 5000, 25, 8);
    random_regular_performance(M * 5000, 25, 10);
    random_regular_performance(M * 3000, 36, 5);
    random_regular_performance(M * 3000, 36, 6);
    random_regular_performance(M * 3000, 36, 7);
    random_regular_performance(M * 3000, 36, 8);
    random_regular_performance(M * 3000, 36, 9);
    random_regular_performance(M * 3000, 36, 10);
    random_regular_performance(M * 1000, 80, 5);
    random_regular_performance(M * 1000, 80, 7);
    random_regular_performance(M * 1000, 80, 9);
    random_regular_performance(M * 1000, 80, 11);
    random_regular_performance(M * 1000, 80, 13);
    random_regular_performance(M * 1000, 80, 15);
    random_regular_performance(M * 100, 400, 10);
    random_regular_performance(M * 100, 400, 12);
    random_regular_performance(M * 100, 400, 14);
    random_regular_performance(M * 100, 400, 16);
    random_regular_performance(M * 100, 400, 18);
    random_regular_performance(M * 100, 400, 20);
    random_regular_performance(M * 6, 10000, 30);
    random_regular_performance(M * 6, 10000, 35);
    random_regular_performance(M * 6, 10000, 40);
    random_regular_performance(M * 6, 10000, 45);
    random_regular_performance(M * 6, 10000, 50);
    random_regular_performance(M * 1, 50000, 30);
    random_regular_performance(M * 1, 50000, 60);
    random_regular_performance(M * 1, 50000, 90);
    random_regular_performance(M * 1, 50000, 120);
}

void test_generator() {
    show("Path undirected 10", path_undirected(10));
    show("Path directed 10", path_directed(10));
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

    show("Random regular 15,6", random_regular(15, 6));
    show("Random regular 16,4 connected", random_regular_connected(16, 4));
    show("Random regular bipartite 10,25,5", random_regular_bipartite(10, 25, 5));

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

    show("Random uniform bipartite 10,20,0.3", random_uniform_bipartite(10, 20, 0.3));
    show("Random uniform bipartite 10,20,0.7", random_uniform_bipartite(10, 20, 0.7));
    show("Random exact bipartite 10,20,73", random_exact_bipartite(10, 20, 73));
    show("Random exact bipartite 10,25,43", random_exact_bipartite(10, 25, 43));

    show("Grid undirected 6x5", grid_undirected(6, 5));
    show("Grid directed 6x5", grid_directed(6, 5));
    show("Grid circular undirected 6x5", circular_grid_undirected(6, 5));
    show("Grid circular directed 6x5", circular_grid_directed(6, 5));
    show("Grid3 undirected 3x4x3", grid3_undirected(3, 4, 3));
    show("Grid3 directed 3x4x3", grid3_directed(3, 4, 3));
    show("Grid3 circular undirected 3x4x3", circular_grid3_undirected(3, 4, 3));
    show("Grid3 circular directed 3x4x3", circular_grid3_directed(3, 4, 3));
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_generator();
    // test_random_regular_performance(1);
    return 0;
}
