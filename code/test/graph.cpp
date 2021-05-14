#include "../formatting.hpp"
#include "../lib/bipartite.hpp"
#include "../lib/circulation.hpp"
#include "../lib/distance.hpp"
#include "../lib/flow.hpp"
#include "../lib/graph_formats.hpp"
#include "../lib/graph_generator.hpp"
#include "test_utils.hpp"

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

void scaling_test_random_regular_run(int T, int n, int k) {
    if (T == 0)
        return;

    START(regular);
    for (int i = 0; i < T; i++) {
        auto g = random_regular(n, k);
    }
    TIME(regular);

    long E = 1L * T * (n * k / 2);
    double per_instance = 1e6 * T / E;

    clear_line();
    print("  -- x{:<6} n={:<6} k={:<4} E={:<9} -- {:>5}ms total -- {:>7.1f}ms per 1M\n",
          T, n, k, E, TIME_MS(regular), per_instance);
}

void scaling_test_random_regular(double M = 1) {
    scaling_test_random_regular_run(int(M * 10), 300, 100);
    scaling_test_random_regular_run(int(M * 10), 300, 200);
    scaling_test_random_regular_run(int(M * 5000), 25, 6);
    scaling_test_random_regular_run(int(M * 5000), 25, 10);
    scaling_test_random_regular_run(int(M * 3000), 36, 6);
    scaling_test_random_regular_run(int(M * 3000), 36, 8);
    scaling_test_random_regular_run(int(M * 3000), 36, 10);
    scaling_test_random_regular_run(int(M * 1000), 80, 5);
    scaling_test_random_regular_run(int(M * 1000), 80, 9);
    scaling_test_random_regular_run(int(M * 1000), 80, 13);
    scaling_test_random_regular_run(int(M * 100), 400, 12);
    scaling_test_random_regular_run(int(M * 100), 400, 16);
    scaling_test_random_regular_run(int(M * 100), 400, 20);
    scaling_test_random_regular_run(int(M * 6), 10000, 30);
    scaling_test_random_regular_run(int(M * 6), 10000, 40);
    scaling_test_random_regular_run(int(M * 6), 10000, 50);
    scaling_test_random_regular_run(int(M * 1), 50000, 60);
    scaling_test_random_regular_run(int(M * 1), 50000, 120);
}

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

void balance_test_generate_flow() {
    // ¹²³⁴⁵⁶⁷⁸⁹⁰ √
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Flow Networks");
        //                      dinitz   mpm   relabel edmonds
        print("{1:<{0}} {2:5} --  V²E     V³     V²√E  VE²lnV  --\n", 58, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(FN_END); i++) {
            long V_sum = 0, E_sum = 0, runs = 0;

            START(fn);
            do {
                auto fn = generate_flow_network(flow_network_kind(i), S);
                V_sum += fn.V, E_sum += fn.E;
            } while (++runs <= 50 && CUR_TIME(fn) < 500ms);
            V_sum /= runs, E_sum /= runs;

            double V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 9 * V + 4 * E + 2 * logV;
            sum_W += W;

            auto& name = flow_kind_name[i];
            print("{:4} {:36} -- V:{:<6} E:{:<8} --", i, name, V_sum, E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * V + E, 3 * V, 2 * V + .5 * E, V + 2 * E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 96, "", sum_W / int(FN_END));
    }
}

void balance_test_generate_circulation() {
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Circulation Networks");
        //                      dinitz   mpm   relabel mincost relabel
        print("{1:<{0}} {2:5} --  V²E     V³     V²√E  V²ElnV  --\n", 58, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(CN_END); i++) {
            long V_sum = 0, E_sum = 0, runs = 0;

            START(fn);
            do {
                auto fn = generate_circulation_network(circulation_network_kind(i), S);
                V_sum += fn.V, E_sum += fn.E;
            } while (++runs <= 50 && CUR_TIME(fn) < 500ms);
            V_sum /= runs, E_sum /= runs;

            double V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 9.5 * V + 3 * E + 2 * logV;
            sum_W += W;

            auto& name = circulation_kind_name[i];
            print("{:4} {:36} -- V:{:<6} E:{:<8} --", i, name, V_sum, E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * V + E, 3 * V, 2 * V + .5 * E, 2 * V + E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 96, "", sum_W / int(CN_END));
    }
}

void balance_test_generate_distance() {
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Distance Graphs");
        print("{1:<{0}} {2:5} --   EV     V³     ElnV  V²lnV  --\n", 58, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(DG_END); i++) {
            long V_sum = 0, E_sum = 0, runs = 0;

            START(fn);
            do {
                auto fn = generate_distance_graph(distance_graph_kind(i), S);
                V_sum += fn.V, E_sum += fn.E;
            } while (++runs <= 50 && CUR_TIME(fn) < 500ms);
            V_sum /= runs, E_sum /= runs;

            double V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 10 * V + 3 * E + 2 * logV;
            sum_W += W;

            auto& name = distance_kind_name[i];
            print("{:4} {:36} -- V:{:<6} E:{:<8} --", i, name, V_sum, E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  V + E, 3 * V, E + logV, 2 * V + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 96, "", sum_W / int(DG_END));
    }
}

void balance_test_generate_bipartite() {
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Bipartite Graphs");
        print("{1:<{0}} {2:5} --  V²E    V³     V²√E  V²ElnV  --\n", 68, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(BG_END); i++) {
            long U_sum = 0, V_sum = 0, E_sum = 0, runs = 0;

            START(fn);
            do {
                auto fn = generate_bipartite_graph(bipartite_graph_kind(i), S);
                U_sum += fn.U, V_sum += fn.V, E_sum += fn.E;
            } while (++runs <= 50 && CUR_TIME(fn) < 500ms);
            V_sum /= runs, E_sum /= runs;

            double U = log2(U_sum), V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 9.5 * (U + V) + 3 * E + 2 * logV;
            sum_W += W;

            auto& name = bipartite_kind_name[i];
            print("{:4} {:36} -- U:{:<6} V:{:<6} E:{:<8} --", i, name, U_sum, V_sum,
                  E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * U + E, 3 * V, 2 * V + .5 * E, 2 * V + E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 105, "", sum_W / int(BG_END));
    }
}

int main() {
    RUN_BLOCK(visual_test_generators());
    RUN_BLOCK(balance_test_generate_distance());
    RUN_BLOCK(balance_test_generate_flow());
    RUN_BLOCK(balance_test_generate_circulation());
    RUN_BLOCK(balance_test_generate_bipartite());
    RUN_BLOCK(scaling_test_random_regular(1));
    return 0;
}
