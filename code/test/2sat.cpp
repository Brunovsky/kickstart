#include "test_utils.hpp"
#include "../algo/2sat.hpp"

inline namespace detail {

/**
 * Generate a solvable 2sat problem with N variables, E edges.
 * Each variable in the canonical solution is true with probability true_p
 * Both variables in each edge are true with probability both_p
 */
edges_t generate_twosat(int N, int E, double true_p = 0.5, double both_p = 0.1) {
    boold distp(true_p);
    boold bothd(both_p);
    boold coind(0.5);
    intd distv(0, N - 1);
    vector<int> hidden_solution(N);
    for (auto& v : hidden_solution) {
        v = distp(mt);
    }
    edges_t g(E);
    for (auto& [u, v] : g) {
        u = distv(mt), v = distv(mt);
        u = hidden_solution[u] ? u : ~u; // 'left' node is true
        if (bothd(mt)) {
            v = hidden_solution[v] ? v : ~v;
        } else {
            v = hidden_solution[v] ? ~v : v;
        }
        if (coind(mt)) {
            swap(u, v);
        }
    }
    return g;
}

bool verify_twosat(const edges_t& g, const vector<int>& assignment) {
    return all_of(begin(g), end(g), [&](auto edge) {
        auto [u, v] = edge;
        return (u >= 0 && assignment[u]) || (u < 0 && !assignment[~u]) ||
               (v >= 0 && assignment[v]) || (v < 0 && !assignment[~v]);
    });
}

} // namespace detail

void unit_test_twosat() {
    edges_t g = {{0, 1}, {~1, 2}, {~0, ~1}, {2, 3}, {~2, 4}, {~3, ~4}, {~2, 3}};
    twosat_scc sat(5);

    for (auto [u, v] : g)
        sat.either(u, v);

    assert(sat.solve());
}

void speed_test_twosat_positive() {
    static vector<int> Ns = {1000, 5000, 15000, 40000, 100000};
    static vector<int> Es = {2, 5, 10, 20, 35};
    const auto duration = 20000ms / (Ns.size() * Es.size());
    map<pair<int, int>, string> table;

    for (int N : Ns) {
        for (int E : Es) {
            START_ACC(sat);

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (duration, now, 100'000, runs) {
                print_time(now, duration, "speed test 2-sat N,E={},{}", N, N * E);

                auto g = generate_twosat(N, N * E);

                START(sat);
                twosat_scc sat(N);
                for (auto [u, v] : g)
                    sat.either(u, v);
                bool ok = sat.solve();
                ADD_TIME(sat);

                assert(ok && verify_twosat(g, sat.assignment));
            }

            table[{E, N}] = FORMAT_EACH(sat, runs);
        }
    }

    print_time_table(table, "2SAT");
}

int main() {
    RUN_SHORT(unit_test_twosat());
    RUN_BLOCK(speed_test_twosat_positive());
    return 0;
}
