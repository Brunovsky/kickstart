#include "../flow/mincost_push_relabel.hpp"
#include "../formatting.hpp"
#include "../generators/circulation.hpp"
#include "test_utils.hpp"

const string DATASET_FILE = "datasets/mincost_circulation.txt";

template <typename MCF, typename Caps, typename Costs>
void add_edges(MCF& mcc, const edges_t& g, const Caps& caps, const Costs& costs) {
    int E = g.size();
    for (int i = 0; i < E; i++) {
        mcc.add(g[i][0], g[i][1], caps[i], costs[i]);
    }
}

struct mincost_circulation_dataset_test_t {
    string name, comment;
    int V, E;
    edges_t g;
    vector<long> cap, supply;
    vector<long> cost;
    long ans;

    void read(istream& in) {
        while (in.peek() == '#') {
            string line;
            getline(in, line);
            comment += line + "\n";
        }
        in >> ws, getline(in, name);
        in >> V >> E >> ans >> ws;
        assert(V > 0 && E > 0);

        g.resize(E), cap.resize(E), cost.resize(E), supply.resize(V);

        for (int e = 0; e < E; e++) {
            in >> g[e][0] >> g[e][1] >> cap[e] >> cost[e] >> ws;
        }
        for (int u = 0; u < V; u++) {
            in >> supply[u];
        }
        assert(!in.bad());
    }

    void run() const {
        mincost_push_relabel mcc(V, supply);
        add_edges(mcc, g, cap, cost);
        bool ok = mcc.feasible();
        if (ok) {
            auto mincost = mcc.mincost_circulation();
            print_line("expected", ans), print_line("actual", mincost);
        } else {
            auto mincost = mcc.mincost_circulation();
            print_line("expected", ans), print_line("actual", mincost);
        }
    }

    static void print_line(string label, long mincost) {
        mincost < 0 ? print("  {:>8}: infeasible\n", label)
                    : print("  {:>8}:   feasible -- {:>5} cost\n", label, mincost);
    }
};

void dataset_test_mincost_circulation() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    while (!file.eof()) {
        mincost_circulation_dataset_test_t test;
        test.read(file);
        test.run();
        file >> ws;
    }
}

int main() {
    RUN_BLOCK(dataset_test_mincost_circulation());
    return 0;
}
