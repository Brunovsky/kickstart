#include "../flow/dinitz_flow.hpp"
#include "../flow/edmonds_karp.hpp"
#include "../flow/mpm_flow.hpp"
#include "../flow/push_relabel.hpp"
#include "../flow/tidal_flow.hpp"
#include "../lib/flow.hpp"
#include "test_utils.hpp"

template <typename MF, typename Caps>
void add_edges(MF& mf, const edges_t& g, const Caps& caps) {
    int E = g.size();
    for (int i = 0; i < E; i++) {
        mf.add(g[i][0], g[i][1], caps[i]);
    }
}

void speed_test_max_flow_run(flow_network_kind i, int S, int T) {
    START_ACC(generation);
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, flow_kind_name[i]);

        START(generation);
        auto network = generate_flow_network(i, S);
        add_cap_flow_network(network, 1, 100'000'000'000);
        ADD_TIME(generation);

        vector<long> mf(3);

        START(dinitz);
        dinitz_flow<> g0(network.V);
        add_edges(g0, network.g, network.cap);
        mf[0] = g0.maxflow(network.s, network.t);
        ADD_TIME(dinitz);

        START(push_relabel);
        push_relabel<> g1(network.V);
        add_edges(g1, network.g, network.cap);
        mf[1] = g1.maxflow(network.s, network.t, true);
        ADD_TIME(push_relabel);

        START(tidal);
        tidal_flow<> g2(network.V);
        add_edges(g2, network.g, network.cap);
        mf[2] = g2.maxflow(network.s, network.t);
        ADD_TIME(tidal);

        if (!all_eq(mf)) {
            fail("Random test failed: {}", fmt::join(mf, " "));
        }
    }

    clear_line();
    print(" speed test {} (S={}, x{}):\n", flow_kind_name[i], S, T);
    PRINT_TIME(generation);
    PRINT_TIME(dinitz);
    PRINT_TIME(push_relabel);
    PRINT_TIME(tidal);
}

void speed_test_max_flow() {
    static constexpr int N = 5;
    static constexpr int sizes[] = {500, 1800, 6000, 12000, 20000};
    static constexpr int amounts[] = {400, 100, 25, 15, 10};
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(FN_END); i++) {
            speed_test_max_flow_run(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
}

void stress_test_max_flow(int T = 10000) {
    intd kindd(0, int(FN_END) - 1);
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test max flow");
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
            fail("Random test failed: {}", fmt::join(mf, " "));
        }
    }
}

int main() {
    RUN_BLOCK(stress_test_max_flow());
    RUN_BLOCK(speed_test_max_flow());
    return 0;
}
