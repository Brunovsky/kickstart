#include "../maximum_flow.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

bool eq(flow_t a, flow_t b, flow_t c) { return a == b && b == c; }
bool eq(flow_t a, flow_t b, flow_t c, flow_t d) { return a == b && eq(b, c, d); }
bool eq(flow_t a, flow_t b, flow_t c, flow_t d, flow_t e) {
    return a == b && eq(b, c, d, e);
}

void speed_test(flow_network_kind i, int S, int T) {
    START_ACC(dinitz);
    START_ACC(push_relabel);
    START_ACC(tidal);
    START_ACC(mpm);

    vector<flow_t> mf[4];
    mf[0] = mf[1] = mf[2] = mf[3] = flows_t(T);

    for (int t = 0; t < T; t++) {
        print("\rspeed test {} {}...", t + 1, flow_kind_name[i]);
        auto network = generate_cap_flow_network(i, S, 100'000);

        START(dinitz);
        dinitz_flow g0(network.V, network.g, network.cap);
        mf[0][t] = g0.maxflow(network.s, network.t);
        ADD_TIME(dinitz);

        START(push_relabel);
        push_relabel g1(network.V, network.g, network.cap);
        mf[1][t] = g1.maxflow(network.s, network.t, true);
        ADD_TIME(push_relabel);

        START(tidal);
        tidal_flow g2(network.V, network.g, network.cap);
        mf[2][t] = g2.maxflow(network.s, network.t);
        ADD_TIME(tidal);

        START(mpm);
        mpm_flow g3(network.V, network.g, network.cap);
        mf[3][t] = g3.maxflow(network.s, network.t);
        ADD_TIME(mpm);
    }

    print("\r speed test {} (S={}, x{}):\n", flow_kind_name[i], S, T);
    PRINT_ACC(dinitz);
    PRINT_ACC(push_relabel);
    PRINT_ACC(tidal);
    PRINT_ACC(mpm);

    if (mf[0] != mf[1] || mf[1] != mf[2] || mf[2] != mf[3]) {
        print("WARNING: Different answers\n");
        for (int c = 0, t = 0; t < T && c < 5; t++) {
            if (!eq(mf[0][t], mf[1][t], mf[2][t], mf[3][t])) {
                print(" -- {:>9} {:>9} {:>9}\n", mf[0][t], mf[1][t], mf[2][t], mf[3][t]);
            }
        }
    }
}

constexpr int N = 5;
constexpr int sizes[] = {500, 1800, 6000, 12000, 20000};
constexpr int amounts[] = {5000, 1000, 100, 40, 10};

void test_speed() {
    for (int n = 0; n < N; n++) {
        for (int i = 0; i < int(FN_END); i++) {
            speed_test(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
}

void test_equal(int i) {
    auto network = generate_cap_flow_network(FN_COMPLETE, 100, 100'000);
    int V = network.V;
    const auto &g = network.g;
    const auto &cap = network.cap;

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

    print("\r{}", string(60, ' '));
    print("\rrandom test #{}: {} {} {} {} {}", i, mf1, mf2, mf3, mf4, mf5);
    if (!eq(mf1, mf2, mf3, mf4, mf5)) {
        print("\nRandom test failed\n");
        exit(1);
    }
}

void test_equals(int R = 100) {
    for (int i = 1; i <= R; i++) {
        test_equal(i);
    }
    print("\n");
}

int main() {
    mt.seed(73);
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_equals();
    test_speed();
    return 0;
}
