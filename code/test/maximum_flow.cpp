#include "../maximum_flow.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

template <typename A, typename... T>
bool eq(A&& a, T&&... args) {
    return ((a == args) && ...);
}

void speed_unit_test(flow_network_kind i, int S, int T) {
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);
    START_ACC(mpm);

    int errors = 0, max_errors = 10;

    for (int t = 0; t < T; t++) {
        print("\rspeed test {} {}...", t + 1, flow_kind_name[i]);
        auto network = generate_cap_flow_network(i, S, 100'000'000'000);
        flow_t mf[4];

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

        if (errors < max_errors && !eq(mf[0], mf[1], mf[2], mf[3])) {
            print(" -- {:>9} {:>9} {:>9} {:>9}\n", mf[0], mf[1], mf[2], mf[3]);
            errors++;
        }
    }

    print("\r speed test {} (S={}, x{}):\n", flow_kind_name[i], S, T);
    PRINT_ACC(dinitz);
    PRINT_ACC(push_relabel);
    PRINT_ACC(tidal);
    PRINT_ACC(mpm);
}

constexpr int N = 5;
constexpr int sizes[] = {500, 1800, 6000, 12000, 20000};
constexpr int amounts[] = {3000, 800, 100, 40, 10};

void test_speed() {
    for (int n = 0; n < N; n++) {
        print("Speed test S={} x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(FN_END); i++) {
            speed_unit_test(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
}

void equal_unit_test(int i) {
    auto network = generate_cap_flow_network(FN_COMPLETE, 100, 100'000);
    int V = network.V;
    const auto& g = network.g;
    const auto& cap = network.cap;

    edmonds_karp flow1(V, g, cap);
    dinitz_flow flow2(V, g, cap);
    push_relabel flow3(V, g, cap);
    tidal_flow flow4(V, g, cap);
    mpm_flow flow5(V, g, cap);

    long mf1 = flow1.maxflow(0, V - 1);
    long mf2 = flow2.maxflow(0, V - 1);
    long mf3 = flow3.maxflow(0, V - 1);
    long mf4 = flow4.maxflow(0, V - 1);
    long mf5 = flow5.maxflow(0, V - 1);
    assert(mf1 != 0);

    print("\r{}", string(80, ' '));
    print("\rrandom test #{}: {} {} {} {} {}", i, //
          mf1, mf2, mf3, mf4, mf5);
    if (!eq(mf1, mf2, mf3, mf4, mf5)) {
        print("\nRandom test failed\n");
        exit(1);
    }
}

void test_equals(int R = 100) {
    for (int i = 1; i <= R; i++) {
        equal_unit_test(i);
    }
    print("\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_equals();
    test_speed();
    return 0;
}
