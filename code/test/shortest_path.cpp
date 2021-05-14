#include "../lib/distance.hpp"
#include "../path/astar.hpp"
#include "../path/bellman_ford.hpp"
#include "../path/dijkstra.hpp"
#include "../path/floyd_warshall.hpp"
#include "../path/johnsons.hpp"
#include "test_utils.hpp"

template <typename SP>
bool compute_all_pairs(const distance_graph& dg, vector<vector<long>>& dist) {
    SP g(dg.V, dg.g, dg.weight);
    bool cycle = !g.compute();
    dist = move(g.dist);
    return cycle;
}

template <typename SP>
bool compute_negative(const distance_graph& dg, int s, vector<long>& dist) {
    SP g(dg.V, dg.g, dg.weight);
    bool cycle = !g.compute(s);
    dist = move(g.dist);
    return cycle;
}

template <typename SP>
void compute_positives(const distance_graph& dg, int s, vector<long>& dist) {
    SP g(dg.V, dg.g, dg.weight);
    g.compute(s);
    dist = move(g.dist);
}

void speed_test_all_pairs_shortest_path_run(distance_graph_kind i, int S, int T) {
    START_ACC(floyd_warshall);
    START_ACC(johnsons);

    int cycles = 0;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, distance_kind_name[i]);
        auto dg = generate_distance_graph(i, S);
        add_weight_distance_graph(dg, -100'000 / S, 100'000'000);

        vector<vector<vector<long>>> dist(2);
        vector<bool> cycle(2, false);

        START(floyd_warshall);
        cycle[0] = compute_all_pairs<floyd_warshall>(dg, dist[0]);
        ADD_TIME(floyd_warshall);

        START(johnsons);
        cycle[1] = compute_all_pairs<johnsons>(dg, dist[1]);
        ADD_TIME(johnsons);

        if (cycle[0] != cycle[1]) {
            fail("negative cycle disagreement\n"
                 "  floyd_warshall: {}\n"
                 "        johnsons: {}\n",
                 cycle[0], cycle[1]);
        }
        if (cycle[0] || cycle[1]) {
            cycles++;
            continue;
        }
        if (!all_eq(dist)) {
            int errors = 0, max_errors = 10;
            for (int u = 0; u < dg.V && errors < max_errors; u++) {
                for (int v = 0; v < dg.V && errors < max_errors; v++) {
                    auto d0 = dist[0][u][v], d1 = dist[1][u][v];
                    if (d0 != d1) {
                        print(" -- {:>3}->{:<3} | {} {}\n", u, v, d0, d1);
                        errors++;
                    }
                }
            }
            fail("distances not equal\n");
        }
    }

    clear_line();
    print(" speed test {} ({:.1f}% neg cycles)\n", distance_kind_name[i],
          100.0 * cycles / T);
    PRINT_TIME(floyd_warshall);
    PRINT_TIME(johnsons);
}

void speed_test_all_pairs_shortest_paths() {
    constexpr int N = 5;
    constexpr int sizes[] = {100, 250, 500, 1400, 2000};
    constexpr int amounts[] = {1000, 100, 10, 5, 2};
    for (int n = 0; n < N; n++) {
        print("speed test group S={} x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(DG_END); i++) {
            speed_test_all_pairs_shortest_path_run(distance_graph_kind(i), sizes[n],
                                                   amounts[n]);
        }
    }
}

void speed_test_negative_shortest_path_run(distance_graph_kind i, int S, int T) {
    START_ACC(bellman_ford);

    int cycles = 0;

    for (int t = 0; t < T; t++) {
        print("\rspeed test {} {}...", t + 1, distance_kind_name[i]);
        auto dg = generate_distance_graph(i, S);
        add_weight_distance_graph(dg, -10'000, 10'000'000);

        vector<vector<long>> dist(1);
        vector<bool> cycle(1, false);

        START(bellman_ford);
        cycle[0] = compute_negative<bellman_ford>(dg, 0, dist[0]);
        ADD_TIME(bellman_ford);
    }

    clear_line();
    print(" speed test {} ({:.1f}% neg cycles)\n", distance_kind_name[i],
          100.0 * cycles / T);
    PRINT_TIME(bellman_ford);
}

void speed_test_negative_shortest_paths() {
    constexpr int N = 5;
    constexpr int sizes[] = {300, 800, 1700, 4000, 10000};
    constexpr int amounts[] = {1000, 100, 10, 5, 2};
    for (int n = 0; n < N; n++) {
        print("Speed test S={} x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(DG_END); i++) {
            speed_test_negative_shortest_path_run(distance_graph_kind(i), sizes[n],
                                                  amounts[n]);
        }
    }
}

void stress_test_positive_shortest_paths(int T = 2000) {
    intd kindd(0, int(DG_END) - 1);
    for (int i = 0; i < T; i++) {
        auto dg = generate_distance_graph(distance_graph_kind(kindd(mt)), 200);
        add_weight_distance_graph(dg, 1, 100'000);
        int V = dg.V, s = 0, t = V - 1;

        vector<long> d1, d2;

        compute_positives<dijkstra>(dg, s, d1);
        compute_positives<bellman_ford>(dg, s, d2);

        print_progress(i, T, format("-- {} {}", d1[t], d2[t]));
        if (d1 != d2) {
            fail("distances not equal");
        }
    }
}

int main() {
    RUN_BLOCK(stress_test_positive_shortest_paths());
    RUN_BLOCK(speed_test_all_pairs_shortest_paths());
    RUN_BLOCK(speed_test_negative_shortest_paths());
    return 0;
}
