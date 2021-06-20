#include "test_utils.hpp"
#include "../flow/mincost_edmonds_karp.hpp"
#include "../lib/graph_generator.hpp"

const string DATASET_FILE = "datasets/mincost_flow.txt";

inline namespace detail {

template <typename MCF, typename Caps, typename Costs>
void add_edges(MCF& mcf, const edges_t& g, const Caps& caps, const Costs& costs) {
    int E = g.size();
    for (int i = 0; i < E; i++) {
        mcf.add(g[i][0], g[i][1], caps[i], costs[i]);
    }
}

} // namespace detail

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
        mincost_edmonds_karp<int, long> mek(V);
        add_edges(mek, g, cap, cost);

        for (int i = 0; i < T; i++) {
            auto F = thresholds[i];
            auto [f0, c0] = mek.mincost_flow(s, t, F);
            mek.clear_flow();

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
}

void speed_test_mincost_flow() {
    static vector<int> Vs = {30, 60, 100, 200, 300, 500};
    static vector<double> pVs = {1.5, 3.0, 7.0, 15.0, 30.0};
    static vector<double> as = {-.35,  -.15, -.05, -.01, -.001, 0,
                                +.001, +.01, +.05, +.15, +.35};
    const auto runtime = 60000ms / (Vs.size() * pVs.size() * as.size());
    map<tuple<pair<int, double>, double, string>, stringable> table;

    auto run = [&](int V, double pV, double alpha) {
        START_ACC2(edmonds, gen);
        double p = min(1.0, pV / V);
        long Esum = 0;

        LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
            print_time(now, runtime, "speed test mincostflow V,p,a={},{:.1e},{}", V, p,
                       alpha);

            START(gen);
            auto [g, s, t] = random_geometric_flow_connected(V, p, p / 2, alpha);
            auto cap = rands_wide<int>(g.size(), 1, 100'000'000, -5);
            auto cost = rands_wide<int>(g.size(), 1, 100'000'000, -5);
            Esum += g.size();
            ADD_TIME(gen);

            START(edmonds);
            mincost_edmonds_karp<int, int, long, long> g0(V);
            add_edges(g0, g, cap, cost);
            auto ans1 = g0.mincost_flow(s, t);
            ADD_TIME(edmonds);

            mincost_edmonds_karp<long, long, long, long> g1(V);
            add_edges(g1, g, cap, cost);
            auto ans2 = g1.mincost_flow(s, t);
            assert(ans1 == ans2);

            g0.clear_flow();
            auto ans3 = g0.mincost_flow(s, t);
            assert(ans1 == ans3);
        }

        table[{{V, alpha}, pV, "gen"}] = FORMAT_EACH(gen, runs);
        table[{{V, alpha}, pV, "edmonds"}] = FORMAT_EACH(edmonds, runs);
    };

    for (int V : Vs) {
        for (double pV : pVs) {
            for (double alpha : as) {
                run(V, pV, alpha);
            }
        }
    }

    print_time_table(table, "Mincost maxflow");
}

int main() {
    RUN_BLOCK(dataset_test_mincost_flow());
    RUN_BLOCK(speed_test_mincost_flow());
    return 0;
}
