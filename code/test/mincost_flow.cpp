#include "../mincost_flow.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

const string UNIT_TESTS = "datasets/mincost_flow.txt";

template <typename MCF>
MCF convert(const cost_flow_graph& g) {
    MCF mcf(g.V);
    for (int e = 0; e < 2 * g.E; e += 2) {
        int u = g.source[e], v = g.target[e], c = g.cap[e], w = g.cost[e];
        mcf.add(u, v, c, w);
    }
    return mcf;
}

struct Test {
    string name, comment;
    cost_flow_graph g;
    vector<long> f;
    int s, t;
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

    int V, E, F;
    in >> ws >> V >> E >> test.s >> test.t >> ws;
    assert(V > 0 && E > 0);
    assert(!in.bad());

    g = cost_flow_graph(V);
    for (int i = 0; i < E; i++) {
        int u, v, c, w;
        in >> u >> v >> c >> w >> ws;
        g.add(u, v, c, w);
    }
    in >> F;
    test.f.resize(F);
    for (int i = 0; i < F; i++) {
        in >> test.f[i] >> ws;
    }
    return test;
}

void read_unit_tests(vector<Test>& tests, istream& in = cin) {
    in >> ws;
    while (!in.eof()) {
        tests.push_back(read_unit_test(in));
        in >> ws;
    }
}

template <typename MCF>
void run_test(Test& test) {
    print("{}", test.comment);
    auto g = convert<MCF>(test.g);
    int s = test.s, t = test.t;
    for (long f : test.f) {
        auto [flow, cost] = g.mincost_flow(s, t, f);
        print("{:>4} -- {:4} {:4}\n", f, flow, cost);
        g.flow.assign(2 * g.E, 0);
    }
    auto [flow, cost] = g.mincost_flow(s, t);
    print("{:>4} -- {:4} {:4}\n", "inf", flow, cost);
}

void run_dataset_tests() {
    vector<Test> tests;
    ifstream file(UNIT_TESTS);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test<mincost_edmonds_karp>);
}

// T = # tests, A = # test subjects, R = # random tests
constexpr int T = 5, A = 1, R = 100;
const char* names[T] = {"sparse flow networks", "dense flow networks",
                        "very dense flow networks", "large sparse flow networks",
                        "huge sparse flow networks"};
int quantity[T] = {1000, 500, 300, 100, 20};
bool hard[T] = {0, 0, 0, 1, 1};
vector<cost_flow_graph> graphs[T];
vector<pair<long, long>> flows[A];

auto make_flow(int V, double p, long max_cap, long max_cost) {
    auto g = random_flow_graph(V, p, max_cap);
    return add_costs(g, max_cost);
}

auto generate(int i) {
    intd distV(30, 50);
    intd largeV(150, 250);
    intd hugeV(800, 1100);
    reald sparse(4.0, 12.0);
    int V;
    switch (i) {
    case 0:
        V = distV(mt);
        return make_flow(V, sparse(mt) / V, 100'000, 100'000);
    case 1:
        V = distV(mt);
        return make_flow(V, 0.25, 100'000, 100'000);
    case 2:
        V = distV(mt);
        return make_flow(V, 1.0, 100'000, 100'000);
    case 3:
        V = largeV(mt);
        return make_flow(V, sparse(mt) / V, 100'000, 100'000);
    case 4:
        V = hugeV(mt);
        return make_flow(V, sparse(mt) / V, 100'000, 100'000);
    default:
        assert(false);
    }
    __builtin_unreachable();
}

void generate_randoms() {
    for (int t = 0; t < T; t++) {
        for (int i = 0; i < quantity[t]; i++) {
            print("\rGenerating {} {}...", names[t], i + 1);
            graphs[t].push_back(generate(t));
        }
        print("\n");
    }
}

template <typename MCF, int k>
void speed_test(const string& name, int t) {
    int G = graphs[t].size();
    flows[k].resize(G);

    auto now = steady_clock::now();
    for (int i = 0; i < G; i++) {
        const auto& f = graphs[t][i];
        int V = f.V;
        MCF mcf(V);
        for (int u = 0; u < V; u++) {
            for (int e : f.adj[u]) {
                mcf.add(u, f.target[e], f.cap[e], f.cost[e]);
            }
        }
        flows[k][i] = mcf.mincost_flow(0, V - 1);
        if (hard[t])
            print("\r{} f={} c={}      ", i + 1, flows[k][i].first, flows[k][i].second);
    }
    auto time = duration_cast<ms>(steady_clock::now() - now);

    print("\r{}\r{}\t{}\n", string(50, ' '), time.count(), name.data());
}

void test_speed() {
    generate_randoms();
    for (int t = 0; t < T; t++) {
        print("--- {}  {}\n", t, names[t]);
        speed_test<mincost_edmonds_karp, 0>("edmonds karp", t);

        print("\n");
    }
}

void test_random(int R) {
    intd distV(100, 150);
    reald moderate(0.3, 0.6);
    mt.seed(73);
    for (int i = 1; i <= R; i++) {
        int V = distV(mt);
        auto g = make_flow(V, moderate(mt), 10000, 10000);
        auto mcf = convert<mincost_edmonds_karp>(g);
        auto [f, c] = mcf.mincost_flow(0, V - 1, 15000);
        print("\rRandom {}... {} {}     ", i, f, c);
    }
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    run_dataset_tests();
    test_random(10000);
    test_speed();
    return 0;
}
