#include "../maximum_flow.hpp"

#include "../graph_generator.hpp"
#include "test_utils.hpp"

// *****

constexpr int N = 5;
constexpr int sizes[] = {500, 1800, 6000, 12000, 20000};
constexpr int amounts[] = {3000, 800, 100, 40, 10};

void test_speed(flow_network_kind i, int S, int T) {
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);
    START_ACC(mpm);

    int errors = 0, max_errors = 10;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, flow_kind_name[i]);
        auto network = generate_flow_network(i, S);
        add_cap_flow_network(network, 100'000'000'000);
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

        if (errors < max_errors && !all_eq(mf)) {
            print(" -- {:>9}\n", fmt::join(mf, " "));
            errors++;
        }
    }

    clear_line();
    print(" speed test {} (S={}, x{}):\n", flow_kind_name[i], S, T);
    PRINT_TIME(dinitz);
    PRINT_TIME(push_relabel);
    PRINT_TIME(tidal);
    PRINT_TIME(mpm);
}

void test_speed() {
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(FN_END); i++) {
            test_speed(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
}

void test_random_equals(int T = 10000) {
    print("random test ");
    for (int t = 0; t < T; t++) {
        auto network = generate_flow_network(FN_COMPLETE, 100);
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

        print_progress(t, T, format("{}", fmt::join(mf, " ")));

        if (!all_eq(mf)) {
            fail("Random test failed (see above)");
        }
    }
    print("\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_random_equals();
    test_speed();
    return 0;
}
