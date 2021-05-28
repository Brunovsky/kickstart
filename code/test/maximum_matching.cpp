#include "test_utils.hpp"
#include "../matching/maximum_matching.hpp"
#include "../lib/bipartite.hpp"
#include "../lib/bipartite_matching.hpp"
#include "../matching/hopcroft_karp.hpp"

inline namespace speed_testing_maximum_matching {

void speed_test_maximum_matching_run(bipartite_graph_kind i, int S) {
    START_ACC(gen);
    START_ACC(mm);
    START_ACC(hopcroft);

    LOOP_FOR_DURATION_TRACKED_RUNS(2s, now, runs) {
        print_time(now, 2s, 50ms, bipartite_kind_name[i]);

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

    printcl(" speed test {} (S={}, x{}):\n", bipartite_kind_name[i], S, runs);
    PRINT_EACH_US(gen, runs);
    PRINT_EACH_US(mm, runs);
    PRINT_EACH_US(hopcroft, runs);
}

void speed_test_maximum_matching() {
    static const vector<int> sizes = {1000, 2500, 8000, 15000, 30000};

    for (int n = 0; n < int(sizes.size()); n++) {
        print("speed test group S={}\n", sizes[n]);
        for (int i = 0; i < int(BG_END); i++) {
            speed_test_maximum_matching_run(bipartite_graph_kind(i), sizes[n]);
        }
    }
}

} // namespace speed_testing_maximum_matching

inline namespace stress_testing_maximum_matching {

void stress_test_maximum_matching() {
    intd distV(90, 600);
    reald distMp(0.1, 1.0);
    reald distEp(3.0, 30.0);

    LOOP_FOR_DURATION_TRACKED(8s, now) {
        print_time(now, 8s, 50ms, "stress test maximum matching");

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
}

} // namespace stress_testing_maximum_matching

inline namespace unit_testing_maximum_matching {

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
}

} // namespace unit_testing_maximum_matching

int main() {
    RUN_SHORT(unit_test_maximum_matching());
    RUN_BLOCK(stress_test_maximum_matching());
    RUN_BLOCK(speed_test_maximum_matching());
    return 0;
}
