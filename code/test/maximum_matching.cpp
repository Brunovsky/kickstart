#include "test_utils.hpp"
#include "../matching/maximum_matching.hpp"
#include "../lib/bipartite_matching.hpp"
#include "../matching/hopcroft_karp.hpp"

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

void stress_test_maximum_matching() {
    intd distV(90, 600);
    reald distMp(0.1, 1.0);
    reald distEp(3.0, 30.0);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test maximum matching");

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

void speed_test_maximum_matching() {
    static vector<int> Vs = {1000, 2500, 8000, 15000, 30000};
    static vector<double> pVs = {5.0, 15.0, 50.0, 120.0};
    static vector<double> Ms = {.01, .05, .25, .50, .75, .99, 1.0};
    const auto runtime = 30000ms / (Vs.size() * pVs.size() * Ms.size());
    map<tuple<pair<int, int>, double, string>, string> table;

    auto run = [&](int V, double p, double M) {
        START_ACC3(gen, mm, hopcroft);

        LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
            print_time(now, runtime, "speed test maximum matching V,M={},{}", V, V * M);

            START(gen);
            auto g = random_bipartite_matching(V, V, M * V, p / V);
            bipartite_matching_hide_topology(V, V, g);
            ADD_TIME(gen);

            START(mm);
            maximum_matching mm(V, V, g);
            int m0 = mm.compute();
            ADD_TIME(mm);

            START(hopcroft);
            hopcroft_karp hk(V, V, g);
            int m1 = hk.compute();
            ADD_TIME(hopcroft);

            assert(m0 == m1);
        }

        table[{{V, p}, M, "gen"}] = FORMAT_EACH(gen, runs);
        table[{{V, p}, M, "mm"}] = FORMAT_EACH(mm, runs);
        table[{{V, p}, M, "hopcroft"}] = FORMAT_EACH(hopcroft, runs);
    };

    for (int V : Vs) {
        for (double p : pVs) {
            for (double M : Ms) {
                run(V, p, M);
            }
        }
    }

    print_time_table(table, "Maximum matching");
}

int main() {
    RUN_SHORT(unit_test_maximum_matching());
    RUN_BLOCK(stress_test_maximum_matching());
    RUN_BLOCK(speed_test_maximum_matching());
    return 0;
}
