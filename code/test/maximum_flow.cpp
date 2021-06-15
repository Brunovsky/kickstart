#include "test_utils.hpp"
#include "../flow/dinitz_flow.hpp"
#include "../flow/edmonds_karp.hpp"
#include "../flow/push_relabel.hpp"
#include "../flow/tidal_flow.hpp"
#include "../lib/flow.hpp"
#include "../lib/graph_formats.hpp"

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
    static const vector<int> sizes = {500, 1800, 6000, 12000, 20000};
    const auto duration = 20000ms / (sizes.size() * FN_END);
    map<tuple<string, int, string>, string> table;

    for (int n = 0; n < int(sizes.size()); n++) {
        for (int i = 0; i < int(FN_END); i++) {
            int S = sizes[n];
            string name = flow_kind_name[i];

            START_ACC4(generation, dinitz, push_relabel, tidal);

            LOOP_FOR_DURATION_TRACKED_RUNS (duration, now, runs) {
                print_time(now, duration, "{} S={}", name, S);

                START(generation);
                auto network = generate_flow_network(flow_network_kind(i), S);
                add_cap_flow_network(network, 1, 10'000'000);
                ADD_TIME(generation);

                vector<long> mf(3);

                START(dinitz);
                dinitz_flow<int, long> g0(network.V);
                add_edges(g0, network.g, network.cap);
                mf[0] = g0.maxflow(network.s, network.t);
                ADD_TIME(dinitz);

                START(push_relabel);
                push_relabel<int, long> g1(network.V);
                add_edges(g1, network.g, network.cap);
                mf[1] = g1.maxflow(network.s, network.t, true);
                ADD_TIME(push_relabel);

                START(tidal);
                tidal_flow<int, long> g2(network.V);
                add_edges(g2, network.g, network.cap);
                mf[2] = g2.maxflow(network.s, network.t);
                ADD_TIME(tidal);

                if (!all_eq(mf)) {
                    fail("Random test failed: {}", fmt::join(mf, " "));
                }
            }

            table[{name, sizes[n], "gen"}] = FORMAT_EACH(generation, runs);
            table[{name, sizes[n], "dinitz"}] = FORMAT_EACH(dinitz, runs);
            table[{name, sizes[n], "push"}] = FORMAT_EACH(push_relabel, runs);
            table[{name, sizes[n], "tidal"}] = FORMAT_EACH(tidal, runs);
        }
    }

    print_time_table(table, "Maximum flow");
}

void stress_test_max_flow() {
    intd kindd(0, int(FN_END) - 1);

    LOOP_FOR_DURATION_TRACKED (3s, now) {
        print_time(now, 3s, "stress test max flow");

        auto network = generate_flow_network(flow_network_kind(kindd(mt)), 50);
        add_cap_flow_network(network, 1, 100'000);
        int V = network.V;

        edmonds_karp g1(V);
        dinitz_flow g2(V);
        push_relabel g3(V);
        tidal_flow g4(V);

        add_edges(g1, network.g, network.cap);
        add_edges(g2, network.g, network.cap);
        add_edges(g3, network.g, network.cap);
        add_edges(g4, network.g, network.cap);

        vector<long> mf(4);
        mf[0] = g1.maxflow(0, V - 1);
        mf[1] = g2.maxflow(0, V - 1);
        mf[2] = g3.maxflow(0, V - 1);
        mf[3] = g4.maxflow(0, V - 1);
        assert(mf[0] != 0);

        if (!all_eq(mf)) {
            ofstream file("debug.txt");
            print(file, "{}", simple_dot(network.g, true));
            fail("Random test failed: {}", fmt::join(mf, " "));
        }
    }
}

int main() {
    RUN_BLOCK(stress_test_max_flow());
    RUN_BLOCK(speed_test_max_flow());
    return 0;
}
