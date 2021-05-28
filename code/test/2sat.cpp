#include "test_utils.hpp"
#include "../algo/2sat.hpp"

inline namespace detail {

/**
 * Generate a solvable 2sat problem with N variables, E edges.
 * Each variable in the canonical solution is true with probability true_p
 */
edges_t generate_twosat(int N, int E, double true_p = 0.5) {
    boold distp(true_p), bothd(0.1), coin(0.5);
    intd distv(0, N - 1);
    vector<int> truth(N);
    for (auto& v : truth) {
        v = distp(mt);
    }
    edges_t g(E);
    for (auto& [u, v] : g) {
        u = distv(mt), v = distv(mt);
        u = truth[u] ? u : ~u; // 'left' node is true
        if (bothd(mt)) {
            v = truth[v] ? v : ~v;
        } else {
            v = truth[v] ? ~v : v;
        }
        if (coin(mt))
            swap(u, v);
    }
    return g;
}

bool verify(const edges_t& g, const vector<int>& assignment) {
    for (auto [u, v] : g) {
        if ((u >= 0 && assignment[u]) || (v >= 0 && assignment[v]))
            continue;
        if ((u < 0 && !assignment[~u]) || (v < 0 && !assignment[~v]))
            continue;
        return false;
    }
    return true;
}

} // namespace detail

inline namespace unit_testing_twosat {

void unit_test_twosat() {
    edges_t g = {{0, 1}, {~1, 2}, {~0, ~1}, {2, 3}, {~2, 4}, {~3, ~4}, {~2, 3}};
    twosat_scc sat(5);

    for (auto [u, v] : g)
        sat.either(u, v);

    assert(sat.solve());
}

} // namespace unit_testing_twosat

inline namespace speed_testing_two_sat {

void speed_test_twosat_positive(int min_N = 1000, int min_E = 3000) {
    intd distN(min_N, 2 * min_N);
    intd distE(min_E, 3 * min_E);

    START_ACC(sat);

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(3s, now, 100'000, runs) {
        print_time(now, 3s, 50ms, "speed test 2-SAT positive");
        int N = distN(mt), E = distE(mt);
        auto g = generate_twosat(N, E);

        START(sat);
        twosat_scc sat(N);
        for (auto [u, v] : g)
            sat.either(u, v);
        bool ok = sat.solve();
        ADD_TIME(sat);

        assert(ok);
        assert(verify(g, sat.assignment));
    }

    int avg_N = 3 * min_N / 2;
    double each = EACH_MS(sat, runs);
    printcl(" {:>8.2f}ms each -- 2sat positive N={}\n", each, avg_N);
}

} // namespace speed_testing_two_sat

int main() {
    RUN_SHORT(unit_test_twosat());
    RUN_BLOCK(speed_test_twosat_positive());
    return 0;
}
