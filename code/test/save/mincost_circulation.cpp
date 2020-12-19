#include "../mincost_circulation.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"

// *****

const string UNIT_TESTS = "datasets/mincost_circulation.txt";

template <typename MCC>
MCC convert(const cost_supply_graph& g) {
    MCC mcf(g.V);
    for (int e = 0; e < 2 * g.E; e += 2) {
        int u = g.source[e], v = g.target[e], c = g.cap[e], w = g.cost[e];
        mcf.add(u, v, c, w);
    }
    mcf.supply = g.supply;
    return mcf;
}

struct Test {
    string name, comment;
    cost_supply_graph g;
};

auto read_unit_test(istream& in) {
    Test test;
    auto& g = test.g;
    while (in.peek() == '#') {
        string line;
        getline(in, line);
        test.comment += line + "\n";
    }
    in >> ws;
    getline(in, test.name);

    int V, E;
    in >> ws >> V >> E >> ws;
    assert(V > 0 && E > 0);

    g = cost_supply_graph(V);
    for (int i = 0; i < E; i++) {
        int u, v, c, w;
        in >> u >> v >> c >> w >> ws;
        g.add(u, v, c, w);
    }
    for (int u = 0; u < V; u++) {
        in >> g.supply[u] >> ws;
    }

    assert(!in.bad());
    return test;
}

void read_unit_tests(vector<Test>& tests, istream& in = cin) {
    in >> ws;
    while (!in.eof()) {
        tests.push_back(read_unit_test(in));
        in >> ws;
    }
}

template <typename MCC>
void run_test(Test& test) {
    print("{}", test.comment);
    auto g = convert<MCC>(test.g);
    bool ok = g.feasible();
    if (ok) {
        auto cost = g.mincost_circulation();
        print("  feasible | cost: {:4}\n", cost);
    } else {
        print("infeasible | ----\n");
    }
    debugn(g.flow);
    debugn(g.cost);
}

void run_dataset_tests() {
    vector<Test> tests;
    ifstream file(UNIT_TESTS);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test<mincost_push_relabel>);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    run_dataset_tests();
    return 0;
}
