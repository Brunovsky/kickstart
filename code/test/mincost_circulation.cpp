#include "test_utils.hpp"
#include "../flow/mincost_push_relabel.hpp"
#include "../flow/network_simplex.hpp"

namespace fs = std::filesystem;
const string DATASET_FOLDER = "datasets/mincost_circulation";

struct mincost_circulation_dataset_test_t {
    using Flow = long;
    using Cost = long;
    struct Edge {
        int node[2];
        Flow lower, upper;
        Cost cost;
    };
    string comment;
    int V = -1, E = 0;
    vector<Edge> g;
    vector<Flow> supply;
    Cost ans = LONG_MIN;

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
                Flow lower, upper;
                Cost cost;
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

    auto& read_netgen(const string& filename) {
        ifstream file(filename);
        return read_netgen(file);
    }

    void run() const {
        mincost_push_relabel<long, long> mpr(V, supply);
        network_simplex<long, long> smp(V);

        for (int e = 0; e < E; e++) {
            mpr.add(g[e].node[0], g[e].node[1], g[e].upper, g[e].cost);
            smp.add(g[e].node[0], g[e].node[1], g[e].lower, g[e].upper, g[e].cost);
        }
        for (int u = 0; u < V; u++) {
            smp.add_supply(u, supply[u]);
        }

        print("===== V={}, E={}\n{}", V, E, comment);

        auto c1 = mpr.mincost_circulation();
        auto feasible = smp.mincost_circulation();
        auto c2 = smp.get_circulation_cost();

        printcl("mincost push relabel: {}\n", c1);
        printcl("     network simplex: {} {}\n", c2, feasible);
        if (ans != LONG_MIN) {
            printcl("expected: {}\n", ans);
        }
    }
};

void dataset_test_mincost_circulation() {
    for (auto entry : fs::directory_iterator(DATASET_FOLDER)) {
        mincost_circulation_dataset_test_t test;
        test.read_netgen(entry.path()).run();
    }
}

int main() {
    RUN_BLOCK(dataset_test_mincost_circulation());
    return 0;
}
