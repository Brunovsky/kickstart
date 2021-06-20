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

} // namespace detail

void speed_test_max_flow() {
    static vector<int> Vs = {10, 20, 35, 50, 100, 200, 500, 1000, 2000, 3500};
    static vector<double> pVs = {2.0, 4.0, 7.0, 15.0, 30.0};
    static vector<double> as = {-.35,  -.15, -.05, -.01, -.001, 0,
                                +.001, +.01, +.05, +.15, +.35};
    const auto duration = 40000ms / (Vs.size() * pVs.size() * as.size());
    map<tuple<pair<int, double>, double, string>, stringable> table;

    auto run = [&](int V, double pV, double alpha) {
        START_ACC4(gen, dinitz, push_relabel, tidal);
        double p = min(1.0, pV / V);
        if (pV > V)
            return;

        LOOP_FOR_DURATION_TRACKED_RUNS (duration, now, runs) {
            print_time(now, duration, "speed test maxflow V,p,a={},{},{}", V, p, alpha);

            START(gen);
            auto [g, s, t] = random_geometric_flow_connected(V, p, p / 2, alpha);
            auto cap = rands_wide<int>(g.size(), 1, 100'000'000, -5);
            ADD_TIME(gen);

            vector<long> mf(3);

            START(dinitz);
            dinitz_flow<int, long> g0(V);
            add_edges(g0, g, cap);
            mf[0] = g0.maxflow(s, t);
            ADD_TIME(dinitz);

            START(push_relabel);
            push_relabel<int, long> g1(V);
            add_edges(g1, g, cap);
            mf[1] = g1.maxflow(s, t, true);
            ADD_TIME(push_relabel);

            START(tidal);
            tidal_flow<int, long> g2(V);
            add_edges(g2, g, cap);
            mf[2] = g2.maxflow(s, t);
            ADD_TIME(tidal);

            if (!all_eq(mf)) {
                ofstream file("debug.txt");
                print(file, "{}", simple_dot(g, true));
                fail("Random test failed: {}", fmt::join(mf, " "));
            }
        }

        table[{{V, alpha}, pV, "dinitz"}] = FORMAT_EACH(dinitz, runs);
        table[{{V, alpha}, pV, "push"}] = FORMAT_EACH(push_relabel, runs);
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
    LOOP_FOR_DURATION_TRACKED (3s, now) {
        print_time(now, 3s, "stress test max flow");

        int V = 50;
        auto [g, s, t] = random_uniform_flow_connected(V, 0.1, 0.05);
        auto cap = rands_unif<int>(g.size(), 1, 100'000);

        edmonds_karp g1(V);
        dinitz_flow g2(V);
        push_relabel g3(V);
        tidal_flow g4(V);

        add_edges(g1, g, cap);
        add_edges(g2, g, cap);
        add_edges(g3, g, cap);
        add_edges(g4, g, cap);

        vector<long> mf(4);
        mf[0] = g1.maxflow(0, V - 1);
        mf[1] = g2.maxflow(0, V - 1);
        mf[2] = g3.maxflow(0, V - 1);
        mf[3] = g4.maxflow(0, V - 1);
        assert(mf[0] != 0);

        if (!all_eq(mf)) {
            ofstream file("debug.txt");
            print(file, "{}", simple_dot(g, true));
            fail("Random test failed: {}", fmt::join(mf, " "));
        }
    }
}

int main() {
    RUN_BLOCK(stress_test_max_flow());
    RUN_BLOCK(speed_test_max_flow());
    return 0;
}
