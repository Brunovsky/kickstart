#include "../matching/maximum_matching.hpp"

#include "../generators/bipartite.hpp"
#include "../generators/bipartite_matching.hpp"
#include "../matching/hopcroft_karp.hpp"
#include "test_utils.hpp"

// *****

void speed_test_maximum_matching_run(bipartite_graph_kind i, int S, int T) {
    START_ACC(gen);
    START_ACC(mm);
    START_ACC(hopcroft);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, bipartite_kind_name[i]);

        START(gen);
        auto network = generate_bipartite_graph(i, S);
        ADD_TIME(gen);

        START(mm);
        maximum_matching mm(network.U, network.V, network.g);
        int m0 = mm.compute();
        ADD_TIME(mm);

        START(hopcroft);
        hopcroft_karp hk(network.U, network.V, network.g);
        int m1 = hk.compute();
        ADD_TIME(hopcroft);

        assert(m0 == m1);
    }

    clear_line();
    print(" speed test {} (S={}, x{}):\n", bipartite_kind_name[i], S, T);
    PRINT_TIME(gen);
    PRINT_TIME(mm);
    PRINT_TIME(hopcroft);
}

void speed_test_maximum_matching() {
    static constexpr int N = 5;
    static constexpr int sizes[] = {1000, 2500, 8000, 15000, 30000};
    static constexpr int amounts[] = {500, 150, 25, 10, 4};
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(BG_END); i++) {
            speed_test_maximum_matching_run(bipartite_graph_kind(i), sizes[n],
                                            amounts[n]);
        }
    }
    print_ok("speed test maximum matching");
}

void stress_test_maximum_matching(int T = 2000) {
    intd distV(90, 600);
    reald distMp(0.1, 1.0);
    reald distEp(3.0, 30.0);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test maximum matching");
        int U = distV(mt), V = distV(mt);
        int M = ceil(distMp(mt) * min(U, V));
        double p = distEp(mt) / (max(U, V));
        auto g = random_bipartite_matching(U, V, M, p);
        bipartite_matching_hide_topology(U, V, g);

        maximum_matching mm(U, V, g);
        int m0 = mm.compute();

        hopcroft_karp hk(U, V, g);
        int m1 = hk.compute();

        assert(M == m0 && M == m1);
    }

    print_ok("stress test maximum matching");
}

void unit_test_maximum_matching() {
    edges_t g;

    g = {{1, 1}, {1, 3}, {2, 4}, {2, 5}, {3, 2}, {3, 3}, {4, 3}, {4, 4}, {5, 3}};
    int mm0 = maximum_matching(6, 6, g).compute();
    int mm1 = hopcroft_karp(6, 6, g).compute();
    assert(mm0 == 5 && mm1 == 5);

    g = {{1, 1}, {1, 4}, {2, 3}, {2, 6}, {2, 7}, {3, 2}, {3, 4}, {3, 5}, {4, 2},
         {4, 7}, {5, 5}, {5, 6}, {5, 7}, {6, 3}, {6, 6}, {7, 6}, {7, 7}};
    mm0 = maximum_matching(8, 8, g).compute();
    mm1 = hopcroft_karp(8, 8, g).compute();
    assert(mm0 == 7 && mm1 == 7);

    print_ok("unit test maximum matching");
}

int main() {
    unit_test_maximum_matching();
    stress_test_maximum_matching();
    speed_test_maximum_matching();
    return 0;
}
