#include "test_utils.hpp"
#include "../flow/dinitz_flow.hpp"
#include "../flow/edmonds_karp.hpp"
#include "../flow/push_relabel.hpp"
#include "../flow/tidal_flow.hpp"
#include "../lib/graph_formats.hpp"
#include "../lib/graph_generator.hpp"

inline namespace detail {

template <typename MF, typename Caps>
void add_edges(MF& mf, const edges_t& g, const Caps& caps) {
    int E = g.size();
    for (int i = 0; i < E; i++) {
        mf.add(g[i][0], g[i][1], caps[i]);
    }
}

template <typename T, typename O = T>
auto mid_cap(int V, const edges_t& g, T low, T high, int repulsion) {
    int E = g.size();
    vector<O> cap(E);
    for (int e = 0; e < E; e++) {
        auto [u, v] = g[e];
        int spread = min(abs(u - V / 2) + abs(v - V / 2), V);
        double factor = (1L * spread * spread) / (V * V);
        T actual_high = low + llround(factor * high);
        cap[e] = rand_wide<T, O>(low, actual_high, repulsion);
    }
    return cap;
}

} // namespace detail

void speed_test_max_flow() {
    static vector<int> Vs = {300, 1000, 2500, 7500};
    static vector<double> pVs = {2.0, 5.0, 12.0, 30.0, 70.0};
    static vector<double> as = {-.35,  -.15, -.05, -.01, -.001, 0,
                                +.001, +.01, +.05, +.15, +.35};
    const auto duration = 120000ms / (Vs.size() * pVs.size() * as.size());
    map<tuple<pair<int, double>, double, string>, stringable> table;

    auto run = [&](int V, double pV, double alpha) {
        double p = pV / V;
        if (p >= 1.0)
            return;

        START_ACC4(dinitz, push, push_full, tidal);

        LOOP_FOR_DURATION_TRACKED_RUNS (duration, now, runs) {
            print_time(now, duration, "speed test maxflow V,p,a={},{:.3f},{:.3f}", V, p,
                       alpha);

            auto [g, s, t] = random_geometric_flow_connected(V, p, p / 2, alpha);
            add_uniform_self_loops(V, g, 0.2);
            auto cap = mid_cap(V, g, 1, 100'000'000, -10);

            vector<long> ans(4);

            ADD_TIME_BLOCK(dinitz) {
                dinitz_flow<int, long> mf(V);
                add_edges(mf, g, cap);
                ans[0] = mf.maxflow(s, t);
            }

            ADD_TIME_BLOCK(push) {
                push_relabel<int, long> mf(V);
                add_edges(mf, g, cap);
                ans[1] = mf.maxflow(s, t, true);
            }

            ADD_TIME_BLOCK(push_full) {
                push_relabel<int, long> mf(V);
                add_edges(mf, g, cap);
                ans[2] = mf.maxflow(s, t, false);
            }

            ADD_TIME_BLOCK(tidal) {
                tidal_flow<int, long> mf(V);
                add_edges(mf, g, cap);
                ans[3] = mf.maxflow(s, t);
            }

            if (!all_eq(ans)) {
                ofstream file("debug.txt");
                print(file, "{}", simple_dot(g, true));
                fail("Random test failed: {}", fmt::join(ans, " "));
            }
        }

        table[{{V, alpha}, pV, "dinitz"}] = FORMAT_EACH(dinitz, runs);
        table[{{V, alpha}, pV, "push"}] = FORMAT_EACH(push, runs);
        table[{{V, alpha}, pV, "full"}] = FORMAT_EACH(push_full, runs);
        table[{{V, alpha}, pV, "tidal"}] = FORMAT_EACH(tidal, runs);
    };

    for (int V : Vs) {
        for (double pV : pVs) {
            for (double alpha : as) {
                run(V, pV, alpha);
            }
        }
    }

    print_time_table(table, "Maximum flow");
}

void stress_test_max_flow() {
    auto make_graph = []() {
        int V = rand_wide<int>(10, 100, -4);
        int k = rand_wide<int>(1, V, -1);
        double p = rand_wide<double>(0.05, 0.9, -2);
        double q = rand_wide<double>(0.05, 0.9, -2);
        double alpha = rand_grav<double>(-.9, .9, 2);
        edges_t g;
        int s = 0, t = V - 1;

        discrete_distribution<int> typed({50, 30, 30, 25, 25, 10, 5, 5});
        int type = typed(mt);
        if (type == 0) {
            tie(g, s, t) = random_geometric_flow_connected(V, p, q, alpha);
        } else if (type == 1) {
            tie(g, s, t) = random_geometric_flow_dag_connected(V, p, alpha);
        } else if (type == 2) {
            g = random_geometric_directed(V, p, alpha);
        } else if (type == 3) {
            g = random_uniform_directed_connected(V, p);
        } else if (type == 4) {
            g = cycle_graph(V);
        } else if (type == 5) {
            g = path_graph(V);
        } else if (type == 6) {
            int n = V / k;
            g = k_connected_complete_graph(n, k);
            V = n * k, t = V - 1;
        } else if (type == 7) {
            int n = V / k;
            g = k_connected_complete2_graph(n, k);
            V = n * k, t = V - 1;
        }

        add_uniform_self_loops(V, g, 0.2);
        auto cap = mid_cap(V, g, 1, 1000, -3);
        return make_tuple(V, g, s, t, cap);
    };

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, "stress test max flow (runs={})", runs);

        auto [V, g, s, t, cap] = make_graph();

        int C = 4;
        dinitz_flow<int, int> g1(V);
        push_relabel<int, int> g2(V);
        push_relabel<int, int> g3(V);
        tidal_flow<int, int> g4(V);
        // edmonds_karp<int, int> g5(V);

        add_edges(g1, g, cap);
        add_edges(g2, g, cap);
        add_edges(g3, g, cap);
        add_edges(g4, g, cap);
        // add_edges(g5, g, cap);

        vector<int> ans(C);
        ans[0] = g1.maxflow(s, t);
        ans[1] = g2.maxflow(s, t);
        ans[2] = g3.maxflow(s, t, false);
        ans[3] = g4.maxflow(s, t);
        // ans[4] = g5.maxflow(s, t);

        if (!all_eq(ans)) {
            ofstream file("debug.txt");
            print(file, "{}", simple_dot(g, true));
            fail("Random test failed: {}", fmt::join(ans, " "));
        }
    }
}

int main() {
    mt.seed(73);
    RUN_BLOCK(stress_test_max_flow());
    RUN_BLOCK(speed_test_max_flow());
    return 0;
}
