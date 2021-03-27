#include "../2sat.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

// *****

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

void unit_test_twosat() {
    // vertex 0 is completely disconnected
    edges_t g = {{0, 1}, {~1, 2}, {~0, ~1}, {2, 3}, {~2, 4}, {~3, ~4}, {~2, 3}};

    twosat_scc sat(5);

    for (auto [u, v] : g)
        sat.either(u, v);

    bool ok = sat.solve();
    assert(ok);
    cout << sat.assignment << endl;

    print_ok("unit test 2-SAT");
}

void speed_test_twosat_positive(int T = 3000) {
    intd distN(100, 300);
    intd distE(1000, 5000);

    START_ACC(sat);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "speed test 2-SAT positive");
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

    clear_line();
    print("speed test 2-SAT positive x{}\n", T);
    PRINT_TIME(sat);
}

int main() {
    unit_test_twosat();
    speed_test_twosat_positive();
    return 0;
}
