#include "../shortest_path.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

template <typename A, typename... T>
bool eq(A&& a, T&&... args) {
    return ((a == args) && ...);
}

namespace all_pairs {

template <typename SP>
bool compute(const distance_graph& dg, vector<vector<long>>& dist) {
    try {
        SP g(dg.V, dg.g, dg.weight);
        g.compute();
        dist = move(g.dist);
        return false;
    } catch (const runtime_error& e) { return true; }
}

void speed_unit_test(distance_graph_kind i, int S, int T) {
    START_ACC(floyd_warshall);
    START_ACC(johnsons);

    int cycles = 0;

    for (int t = 0; t < T; t++) {
        print("\rspeed test {} {}...", t + 1, distance_kind_name[i]);
        auto dg = generate_distance_weighted_graph(i, S, 100'000'000, -100'000 / S);

        vector<vector<long>> dist[2];
        bool cycle[2]{};

        START(floyd_warshall);
        cycle[0] = compute<floyd_warshall>(dg, dist[0]);
        ADD_TIME(floyd_warshall);

        START(johnsons);
        cycle[1] = compute<johnsons>(dg, dist[1]);
        ADD_TIME(johnsons);

        if (cycle[0] != cycle[1]) {
            print("  -- error: negative cycle disagreement\n");
        }
        if (cycle[0] || cycle[1]) {
            cycles++;
            continue;
        }
        if (!eq(dist[0], dist[1])) {
            int errors = 0, max_errors = 10;
            print(" -- error: distances not equal\n");
            for (int u = 0; u < dg.V && errors < max_errors; u++) {
                for (int v = 0; v < dg.V && errors < max_errors; v++) {
                    auto d0 = dist[0][u][v], d1 = dist[1][u][v];
                    if (d0 != d1) {
                        print("\r -- {:>3}->{:<3} | {} {}\n", u, v, d0, d1);
                        errors++;
                    }
                }
            }
        }
    }

    print("\r speed test {} (S={}, x{}):\n", distance_kind_name[i], S, T);
    print(" -> negative cycles: {}\n", cycles);
    PRINT_ACC(floyd_warshall);
    PRINT_ACC(johnsons);
}

constexpr int N = 5;
constexpr int sizes[] = {100, 250, 500, 1400, 2000};
constexpr int amounts[] = {1000, 100, 10, 5, 2};

void test_speed() {
    for (int n = 0; n < N; n++) {
        print("Speed test S={} x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(DG_END); i++) {
            speed_unit_test(distance_graph_kind(i), sizes[n], amounts[n]);
        }
    }
}

} // namespace all_pairs

namespace negatives {

template <typename SP>
bool compute(const distance_graph& dg, int s, vector<long>& dist) {
    try {
        SP g(dg.V, dg.g, dg.weight);
        g.compute(s);
        dist = move(g.dist);
        return false;
    } catch (const runtime_error& e) { return true; }
}

void speed_unit_test(distance_graph_kind i, int S, int T) {
    START_ACC(bellman_ford);
    START_ACC(goldberg_radzik);

    int cycles = 0;

    for (int t = 0; t < T; t++) {
        print("\rspeed test {} {}...", t + 1, distance_kind_name[i]);
        auto dg = generate_distance_weighted_graph(i, S, 10'000'000, -10'000);

        vector<long> dist[2];
        bool cycle[2]{};

        START(bellman_ford);
        cycle[0] = compute<bellman_ford>(dg, 0, dist[0]);
        ADD_TIME(bellman_ford);

        START(goldberg_radzik);
        cycle[1] = compute<goldberg_radzik>(dg, 0, dist[1]);
        ADD_TIME(goldberg_radzik);

        if (cycle[0] != cycle[1]) {
            print("  -- error: negative cycle disagreement\n");
        }
        if (cycle[0] || cycle[1]) {
            cycles++;
            continue;
        }
        if (!eq(dist[0], dist[1])) {
            int errors = 0, max_errors = 10;
            print(" -- error: distances not equal\n");
            for (int u = 0; u < dg.V && errors < max_errors; u++) {
                auto d0 = dist[0][u], d1 = dist[1][u];
                if (d0 != d1) {
                    print("\r -- ->{:<3} | {} {}\n", u, d0, d1);
                    errors++;
                }
            }
        }
    }

    print("\r speed test {} (S={}, x{}):\n", distance_kind_name[i], S, T);
    print(" -> negative cycles: {}\n", cycles);
    PRINT_ACC(bellman_ford);
    PRINT_ACC(goldberg_radzik);
}

constexpr int N = 5;
constexpr int sizes[] = {300, 800, 1700, 4000, 10000};
constexpr int amounts[] = {1000, 100, 10, 5, 2};

void test_speed() {
    for (int n = 0; n < N; n++) {
        print("Speed test S={} x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(DG_END); i++) {
            speed_unit_test(distance_graph_kind(i), sizes[n], amounts[n]);
        }
    }
}

void test_random_equals(int R = 2000) {
    int cycles = 0;

    for (int i = 1; i <= R; i++) {
        auto dg = generate_distance_weighted_graph(DG_UNIFORM_SPARSE, 200, 100'000, -300);
        int V = dg.V, s = 0, t = V - 1;

        vector<long> d1, d2;
        bool c1, c2;

        c1 = compute<bellman_ford>(dg, s, d1);
        c2 = compute<goldberg_radzik>(dg, s, d2);

        if (c1 != c2) {
            print("-- error: negative cycle disagreement\n");
            break;
        }
        if (c1) {
            cycles++;
            continue;
        }
        long t1 = d1[t], t2 = d2[t];

        print("\r{}", string(80, ' '));
        print("\rrandom test #{}: {} {}", i, t1, t2);
        if (!eq(t1, t2)) {
            print("-- error: different distances\n");
            break;
        }
    }

    print("\n -> cycles: {}  ({}%)\n", cycles, 100.0 * cycles / R);
}

} // namespace negatives

namespace positives {

template <typename SP>
void compute(const distance_graph& dg, int s, vector<long>& dist) {
    SP g(dg.V, dg.g, dg.weight);
    g.compute(s);
    dist = move(g.dist);
}

void test_random_equals(int R = 2000) {
    for (int i = 1; i <= R; i++) {
        auto dg = generate_distance_weighted_graph(DG_MULTIPARTITE, 200, 100'000, 1);
        int V = dg.V, s = 0, t = V - 1;

        vector<long> d1, d2;

        compute<dijkstra>(dg, s, d1);
        compute<bellman_ford>(dg, s, d2);

        long t1 = d1[t], t2 = d2[t];

        print("\r{}", string(80, ' '));
        print("\rrandom test #{}: {} {}", i, t1, t2);
        if (!eq(d1, d2)) {
            print("\nRandom test failed\n");
            exit(1);
        }
    }
    print("\n");
}

} // namespace positives

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    negatives::test_random_equals();
    negatives::test_speed();
    positives::test_random_equals();
    all_pairs::test_speed();
    return 0;
}
