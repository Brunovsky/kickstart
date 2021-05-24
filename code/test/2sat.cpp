#include "test_utils.hpp"
#include "../algo/2sat.hpp"

inline namespace detail {

edges_t generate_twosat(int N, int E, double p = 0.5) {
    boold distp(p), bothd(0.1), coin(0.5);
    intd distv(0, N - 1);
    vector<int> truth(N);
    for (auto& v : truth) {
        v = distp(mt);
    }
    edges_t g(E);
    for (auto& [u, v] : g) {
        u = distv(mt), v = distv(mt);
        u = truth[u] ? u : ~u;
        if (bothd(mt))
            v = truth[v] ? v : ~v;
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
    // vertex 0 is completely disconnected
    edges_t g = {{0, 1}, {~1, 2}, {~0, ~1}, {2, 3}, {~2, 4}, {~3, ~4}, {~2, 3}};
    twosat_scc sat(5);

    for (auto [u, v] : g)
        sat.either(u, v);

    assert(sat.solve());
}

} // namespace unit_testing_twosat

inline namespace speed_testing_two_sat {

void speed_test_twosat_positive(ms runtime = 2s) {
    intd distN(1000, 3000);
    intd distE(2000, 5000);

    START_ACC(sat);
    LOOP_FOR_DURATION_OR_RUNS_TRACKED(runtime, now, 100'000, runs) {
        print_time(now, runtime, 50ms, "speed test 2-SAT positive");
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
    double each = 1e-3 * TIME_US(sat) / runs;
    print_clear(" {:>8.2f}ms -- 2sat positive x{}\n", each, runs);
}

} // namespace speed_testing_two_sat

int main() {
    RUN_SHORT(unit_test_twosat());
    RUN_BLOCK(speed_test_twosat_positive());
    return 0;
}
