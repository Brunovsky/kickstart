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

inline namespace speed_testing_maxflow {

void speed_test_max_flow_run(flow_network_kind i, int S) {
    START_ACC(generation);
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);

    LOOP_FOR_DURATION_TRACKED_RUNS(1s, now, runs) {
        print_time(now, 1s, 50ms, flow_kind_name[i]);

        START(generation);
        auto network = generate_flow_network(i, S);
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

    printcl(" speed test {} (S={}, x{}):\n", flow_kind_name[i], S, runs);
    PRINT_EACH_US(generation, runs);
    PRINT_EACH_US(dinitz, runs);
    PRINT_EACH_US(push_relabel, runs);
    PRINT_EACH_US(tidal, runs);
}

void speed_test_max_flow() {
    static const vector<int> sizes = {500, 1800, 6000, 12000, 20000};

    for (int n = 0; n < int(sizes.size()); n++) {
        print("speed test group S={}, x{}\n", sizes[n]);
        for (int i = 0; i < int(FN_END); i++) {
            speed_test_max_flow_run(flow_network_kind(i), sizes[n]);
        }
    }
}

} // namespace speed_testing_maxflow

inline namespace stress_testing_maxflow {

void stress_test_max_flow() {
    intd kindd(0, int(FN_END) - 1);

    LOOP_FOR_DURATION_TRACKED(10s, now) {
        print_time(now, 10s, 50ms, "stress test max flow");

        auto network = generate_flow_network(flow_network_kind(kindd(mt)), 100);
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

} // namespace stress_testing_maxflow

int main() {
    RUN_BLOCK(stress_test_max_flow());
    RUN_BLOCK(speed_test_max_flow());
    return 0;
}
