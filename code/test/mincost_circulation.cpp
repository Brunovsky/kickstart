#include "test_utils.hpp"
#include "../flow/mincost_push_relabel.hpp"
#include "../flow/network_simplex.hpp"
#include "../flow/feasible_circulation.hpp"
#include "../lib/graph_generator.hpp"

namespace fs = std::filesystem;
const string DATASET_FOLDER = "datasets/mincost_circulation";

inline namespace detail {

#define min3(a, b, c) min(min(a, b), c)
#define max3(a, b, c) max(max(a, b), c)

template <typename Flow, typename Cost>
struct MccEdge {
    array<int, 2> node;
    Flow lower, upper;
    Cost cost;
};

template <typename MCC>
void verify_potentials(int E, const MCC& mcc) {
    for (int e = 0; e < E; e++) {
        assert(mcc.reduced_cost(e) >= 0);
    }
}

template <typename Flow>
auto generate_feasible(int V, const vector<array<int, 2>>& g, array<Flow, 2> flowrange,
                       int repulsion = -10) {
    int E = g.size();
    auto flow1 = rands_wide<Flow>(E, flowrange[0], flowrange[1], repulsion);
    auto flow2 = rands_wide<Flow>(E, flowrange[0], flowrange[1], repulsion);
    auto flow3 = rands_wide<Flow>(E, flowrange[0], flowrange[1], repulsion);
    vector<Flow> lower(E), upper(E), flow(E), supply(V);
    for (int e = 0; e < E; e++) {
        lower[e] = min3(flow1[e], flow2[e], flow3[e]);
        upper[e] = max3(flow1[e], flow2[e], flow3[e]);
        flow[e] = lower[e] ^ upper[e] ^ flow1[e] ^ flow2[e] ^ flow3[e];
        auto [u, v] = g[e];
        supply[u] += flow[e];
        supply[v] -= flow[e];
    }
    return make_tuple(move(lower), move(upper), move(flow), move(supply));
}

} // namespace detail

struct mincost_circulation_dataset_test_t {
    using Edge = MccEdge<long, long>;
    string comment, filename;
    int V = -1, E = 0;
    vector<Edge> g;
    vector<long> supply;
    long ans = LONG_MIN;

    auto& read_netgen(istream& in) {
        int e = 0;
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string type;
            ss >> type;
            if (type == "c" || type == "#") {
                comment += "== " + line.substr(1) + "\n";
            } else if (type == "p") {
                string want;
                ss >> want >> V >> E;
                assert(want == "min");
                g.resize(E);
                supply.resize(V);
            } else if (type == "n") {
                int u;
                ss >> u, u--;
                assert(0 <= u && u < V);
                ss >> supply[u];
            } else if (type == "a") {
                int u, v;
                long lower, upper;
                long cost;
                ss >> u >> v >> lower >> upper >> cost, u--, v--;
                assert(0 <= u && u < V);
                assert(0 <= v && v < V);
                g[e++] = {{u, v}, lower, upper, cost};
                assert(lower == 0);
            } else if (type == "ans") {
                ss >> ans;
            } else {
                throw runtime_error("Unknown netgen row class " + type);
            }
        }
        assert(e == E);
        return *this;
    }

    bool check_feasible() const {
        vector<array<int, 2>> edges(E);
        vector<long> lower(E), upper(E);
        for (int e = 0; e < E; e++) {
            edges[e] = g[e].node;
            lower[e] = g[e].lower;
            upper[e] = g[e].upper;
        }
        return feasible_circulation(edges, lower, upper, supply).has_value();
    }

    auto& read_netgen(const string& filename) {
        ifstream file(this->filename = filename);
        return read_netgen(file);
    }

    void run() const {
        bool is_feasible = check_feasible();
        printcl("===== V={}, E={}, feasible={} {}\n", V, E, is_feasible, filename);

        bool push_feasible = true, netw_feasible = true;
        long push_cost = -1, netw_cost = -1;

        if (V <= 5000) {
            mincost_push_relabel<long, long> push(V, supply);
            TIME_BLOCK(preflow) {
                for (int e = 0; e < E; e++) {
                    push.add(g[e].node[0], g[e].node[1], g[e].upper - g[e].lower,
                             g[e].cost);
                }
                push_feasible = push.feasible();
                push_cost = push_feasible ? push.mincost_circulation() : -1;
            }
        }

        if (V <= 50'000'000) {
            network_simplex<long, long> netw(V);
            TIME_BLOCK(network) {
                for (int e = 0; e < E; e++) {
                    netw.add(g[e].node[0], g[e].node[1], g[e].lower, g[e].upper,
                             g[e].cost);
                }
                for (int u = 0; u < V; u++) {
                    netw.add_supply(u, supply[u]);
                }
                netw_feasible = netw.mincost_circulation();
                netw_cost = netw.get_circulation_cost();
            }
            if (netw_feasible) {
                netw.verify();
            }
        }

        printcl("\tnetwork: {} {}\n", netw_cost, netw_feasible);
        printcl("\tpreflow: {} {}\n", push_cost, push_feasible);

        if (ans != LONG_MIN) {
            printcl("expected: {}\n", ans);
        }
    }
};

void dataset_test_mincost_circulation() {
    regex filepattern("^.*\\.min$", regex_constants::ECMAScript);
    vector<string> files;
    for (auto entry : fs::directory_iterator(DATASET_FOLDER)) {
        if (regex_match(entry.path().string(), filepattern)) {
            files.push_back(entry.path().string());
        }
    }
    sort(begin(files), end(files));
    for (string file : files) {
        mincost_circulation_dataset_test_t test;
        test.read_netgen(file).run();
    }
}

void random_stress_test_mincost_circulation() {
    auto make_graph = []() {
        int V = rand_wide<int>(10, 100, -4);
        int k = rand_wide<int>(1, V, -1);
        double p = rand_wide<double>(0.05, 0.9, -2);
        double q = rand_wide<double>(0.05, 0.9, -2);
        double alpha = rand_grav<double>(-.9, .9, 2);
        edges_t g;
        int s = 0, t = V - 1;

        discrete_distribution<int> typed({50, 30, 30, 25, 25, 10, 5, 5});
        int type = typed(mt);
        if (type == 0) {
            tie(g, s, t) = random_geometric_flow_connected(V, p, q, alpha);
        } else if (type == 1) {
            tie(g, s, t) = random_geometric_flow_dag_connected(V, p, alpha);
        } else if (type == 2) {
            g = random_geometric_directed(V, p, alpha);
        } else if (type == 3) {
            g = random_uniform_directed_connected(V, p);
        } else if (type == 4) {
            g = cycle_graph(V);
        } else if (type == 5) {
            g = path_graph(V);
        } else if (type == 6) {
            int n = V / k;
            g = k_connected_complete_graph(n, k);
            V = n * k, t = V - 1;
        } else if (type == 7) {
            int n = V / k;
            g = k_connected_complete2_graph(n, k);
            V = n * k, t = V - 1;
        }

        int E = g.size();
        auto cost = rands_wide<int>(E, -50'000, 100'000, 15);
        auto [lower, upper, flow, sup] = generate_feasible<int>(V, g, {0, 100'000}, -15);
        return make_tuple(V, E, g, lower, upper, flow, sup, cost);
    };

    LOOP_FOR_DURATION_TRACKED_RUNS (10s, now, runs) {
        print_time(now, 10s, "stress test mincost circulation runs={}", runs);

        auto [V, E, g, lower, upper, flow, supply, cost] = make_graph();

        network_simplex<int, long, long> netw(V);
        for (int u = 0; u < V; u++) {
            netw.add_supply(u, supply[u]);
        }
        for (int e = 0; e < E; e++) {
            auto [u, v] = g[e];
            netw.add(u, v, lower[e], upper[e], cost[e]);
        }

        bool feasible = netw.mincost_circulation();

        assert(feasible);
        netw.verify();
    }
}

void speed_test_mincost_circulation() {
    static vector<int> Vs = {100, 300, 800, 1800, 4000};
    static vector<double> pVs = {2.0, 5.0, 12.0, 30.0, 70.0};
    static vector<double> as = {-.35,  -.15, -.05, -.01, -.001, 0,
                                +.001, +.01, +.05, +.15, +.35};
    const auto duration = 60000ms / (Vs.size() * pVs.size() * as.size());
    map<tuple<pair<int, double>, double, string>, stringable> table;

    auto make_graph = [](int V, double p, double alpha) {
        edges_t g = random_geometric_directed(V, p, alpha);
        add_uniform_self_loops(V, g, 0.2);
        int E = g.size();
        auto cost = rands_wide<int>(E, -50'000, 100'000, 15);
        auto [lower, upper, flow, sup] = generate_feasible<int>(V, g, {0, 100'000}, -15);
        return make_tuple(E, g, lower, upper, flow, sup, cost);
    };

    auto run = [&](int V, double pV, double alpha) {
        double p = pV / V;
        if (p >= 1.0)
            return;

        START_ACC(network);

        LOOP_FOR_DURATION_TRACKED_RUNS (duration, now, runs) {
            print_time(now, duration, "speed test mincost circ. V,p,a={},{:.3f},{:.3f}",
                       V, p, alpha);

            auto [E, g, lower, upper, flow, supply, cost] = make_graph(V, p, alpha);

            vector<long> ans(2);

            ADD_TIME_BLOCK(network) {
                network_simplex<int, long, long> netw(V);
                for (int e = 0; e < E; e++) {
                    netw.add(g[e][0], g[e][1], lower[e], upper[e], cost[e]);
                }
                for (int u = 0; u < V; u++) {
                    netw.add_supply(u, supply[u]);
                }
                netw.mincost_circulation();
                ans[0] = netw.get_circulation_cost();
            }

            // assert(ans[0] == ans[1]);
        }

        table[{{V, alpha}, pV, "network"}] = FORMAT_EACH(network, runs);
    };

    for (int V : Vs) {
        for (double pV : pVs) {
            for (double alpha : as) {
                run(V, pV, alpha);
            }
        }
    }

    print_time_table(table, "Mincost circulation");
}

int main() {
    mt.seed(73);
    RUN_BLOCK(dataset_test_mincost_circulation());
    RUN_BLOCK(random_stress_test_mincost_circulation());
    RUN_BLOCK(speed_test_mincost_circulation());
    return 0;
}
