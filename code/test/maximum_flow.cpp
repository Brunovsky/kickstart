#include "../maximum_flow.hpp"

#include "../gen/flow.hpp"
#include "test_utils.hpp"

// *****

void speed_test_max_flow_run(flow_network_kind i, int S, int T) {
    START_ACC(generation);
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);
    START_ACC(mpm);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, flow_kind_name[i]);

        START(generation);
        auto network = generate_flow_network(i, S);
        add_cap_flow_network(network, 100'000'000'000);
        ADD_TIME(generation);

        vector<flow_t> mf(4);

        START(dinitz);
        dinitz_flow g0(network.V, network.g, network.cap);
        mf[0] = g0.maxflow(network.s, network.t);
        ADD_TIME(dinitz);

        START(push_relabel);
        push_relabel g1(network.V, network.g, network.cap);
        mf[1] = g1.maxflow(network.s, network.t, true);
        ADD_TIME(push_relabel);

        START(tidal);
        tidal_flow g2(network.V, network.g, network.cap);
        mf[2] = g2.maxflow(network.s, network.t);
        ADD_TIME(tidal);

        START(mpm);
        mpm_flow g3(network.V, network.g, network.cap);
        mf[3] = g3.maxflow(network.s, network.t);
        ADD_TIME(mpm);

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
    PRINT_TIME(mpm);
}

void speed_test_max_flow() {
    static constexpr int N = 5;
    static constexpr int sizes[] = {500, 1800, 6000, 12000, 20000};
    static constexpr int amounts[] = {1500, 400, 50, 20, 8};
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(FN_END); i++) {
            speed_test_max_flow_run(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
    print_ok("speed test max flow");
}

void stress_test_max_flow(int T = 10000) {
    intd kindd(0, int(FN_END) - 1);
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test max flow");
        auto network = generate_flow_network(flow_network_kind(kindd(mt)), 100);
        add_cap_flow_network(network, 100'000);
        int V = network.V;
        const auto& g = network.g;
        const auto& cap = network.cap;

        edmonds_karp flow1(V, g, cap);
        dinitz_flow flow2(V, g, cap);
        push_relabel flow3(V, g, cap);
        tidal_flow flow4(V, g, cap);
        mpm_flow flow5(V, g, cap);

        vector<flow_t> mf(5);
        mf[0] = flow1.maxflow(0, V - 1);
        mf[1] = flow2.maxflow(0, V - 1);
        mf[2] = flow3.maxflow(0, V - 1);
        mf[3] = flow4.maxflow(0, V - 1);
        mf[4] = flow5.maxflow(0, V - 1);
        assert(mf[0] != 0);

        if (!all_eq(mf)) {
            fail("Random test failed: {}", fmt::join(mf, " "));
        }
    }
    print_ok("stress test max flow");
}

int main() {
    stress_test_max_flow();
    speed_test_max_flow();
    return 0;
}
