#include "../flow/mincost_edmonds_karp.hpp"
#include "../formatting.hpp"
#include "../generators/flow.hpp"
#include "test_utils.hpp"

// *****

const string DATASET_FILE = "datasets/mincost_flow.txt";

/**
 * # Comment
 * # Comment
 * Name
 * V E T s t
 * u0 v0 cap0 cost0               -+
 * u1 v1 cap1 cost1                |  edges
 * ...                             |  (cap before cost)
 * un vn capn costn               -+
 * F0 cost0                       -+
 * F1 cost1                        |  thresholds
 * ...                             |  and cost solutions
 * Fm costm                       -+
 * cost                           -|  cost solution with no threshold
 */
struct flow_dataset_test_t {
    string name, comment;
    edges_t g;
    int V, E, T, s, t;
    vector<long> cap, cost;
    vector<long> thresholds; // F argument for flow upper cap
    vector<long> ans;
    long max_flow, min_cost;

    void read(istream& in) {
        while (in.peek() == '#') {
            string line;
            getline(in, line);
            comment += line + "\n";
        }
        in >> ws, getline(in, name);
        in >> V >> E >> T >> s >> t >> ws;

        assert(V > 0 && E > 0 && T >= 0 && !in.bad());
        assert(s >= 0 && t >= 0 && s < V && t < V);

        g.resize(E), cap.resize(E), cost.resize(E);
        thresholds.resize(T), ans.resize(T + 1);

        for (int e = 0; e < E; e++) {
            in >> g[e][0] >> g[e][1] >> cap[e] >> cost[e] >> ws;
        }
        for (int i = 0; i < T; i++) {
            in >> thresholds[i] >> ans[i];
        }
        in >> max_flow >> min_cost;
    }

    void run() const {
        mincost_edmonds_karp mek(V, g, cap, cost);
        for (int i = 0; i < T; i++) {
            auto F = thresholds[i];
            auto [f0, c0] = mek.mincost_flow(s, t, F);
            mek.flow.assign(2 * E, 0);
            if (c0 != ans[i]) {
                print(" {} -- V={} E={} -- threshold {} (F={})\n", name, V, E, i, F);
                print("   expected: {:>12} flow | {:>12} cost\n", F, ans[i]);
                print("     actual: {:>12} flow | {:>12} cost\n", f0, c0);
            }
        }
        auto [f0, c0] = mek.mincost_flow(s, t);
        if (f0 != max_flow || c0 != min_cost) {
            print(" {} -- V={} E={} -- uncapped\n", name, V, E);
            print("   expected: {:>12} flow | {:>12} cost\n", max_flow, min_cost);
            print("     actual: {:>12} flow | {:>12} cost\n", f0, c0);
        }
    }
};

void dataset_test_mincost_flow() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    while (!file.eof()) {
        flow_dataset_test_t test;
        test.read(file);
        test.run();
        file >> ws;
    }
    print_ok("dataset test mincost flow");
}

void speed_test_mincost_flow_run(flow_network_kind i, int S, int T) {
    START_ACC(edmonds_karp);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, flow_kind_name[i]);
        auto network = generate_flow_network(i, S);
        add_cap_flow_network(network, 100'000'000'000);
        add_cost_flow_network(network, 100'000'000);

        START(edmonds_karp);
        mincost_edmonds_karp g0(network.V, network.g, network.cap, network.cost);
        g0.mincost_flow(network.s, network.t);
        ADD_TIME(edmonds_karp);
    }

    clear_line();
    print(" {:>8}ms -- edmonds_karp -- {}\n", TIME_MS(edmonds_karp), flow_kind_name[i]);
}

void speed_test_mincost_flow() {
    static constexpr int N = 5;
    static constexpr int sizes[] = {100, 250, 800, 1500, 3000};
    static constexpr int amounts[] = {200, 60, 10, 4, 1};
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x{}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(FN_END); i++) {
            speed_test_mincost_flow_run(flow_network_kind(i), sizes[n], amounts[n]);
        }
    }
    print_ok("speed test mincost flow");
}

int main() {
    dataset_test_mincost_flow();
    speed_test_mincost_flow();
    return 0;
}
