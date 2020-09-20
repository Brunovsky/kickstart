#include "../mincost_flow.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

const string UNIT_TESTS = "datasets/mincost_flow.txt";

struct Test {
    string name, comment;
    edges_t g;
    vector<long> f, caps, costs;
    int V, E, s, t;
};

auto read_unit_test(istream& in) {
    Test test;
    while (in.peek() == '#') {
        string line;
        getline(in, line);
        test.comment += line + "\n";
    }
    in >> ws;
    getline(in, test.name);

    int V, E, F;
    in >> ws >> V >> test.s >> test.t >> ws;
    assert(V > 0 && E > 0);
    assert(!in.bad());
    test.V = V, test.E = E;

    for (int i = 0; i < E; i++) {
        int u, v, c, w;
        in >> u >> v >> c >> w >> ws;
        test.g.push_back({u, v});
        test.caps.push_back(c);
        test.costs.push_back(w);
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
    MCF g(test.V, test.g, test.caps, test.costs);
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
vector<edges_t> graphs;
vector<vector<long>> caps, costs;
vector<pair<long, long>> flows[A];

int generate(int i, edges_t& g, caps_t& caps, costs_t& costs, long max_cap = 99,
             long max_cost) {
    intd smallV(30, 50);
    intd distV(200, 300);
    intd largeV(600, 800);
    intd hugeV(1200, 1500);
    intd completeV(600, 900);
    intd gridV(10, 20);
    reald sparse(5.0, 12.0);
    reald dense(0.3, 0.6);
    intd rankd;
    int V = -1, ranks, m, X, Y, Z;
    double p;
    switch (i) {
    case -1:
        V = smallV(mt), p = min(1.0, sparse(mt) / sqrt(V));
        rankd = intd(3, max(3, V / 6)), ranks = rankd(mt), m = min(8, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 0:
        V = distV(mt), p = sparse(mt) / V;
        rankd = intd(3, max(3, V / 6)), ranks = rankd(mt), m = min(8, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 1:
        V = distV(mt), p = sparse(mt) / V;
        rankd = intd(5, min(V, 20)), ranks = rankd(mt), m = min(20, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 2:
        V = distV(mt), p = min(1.0, sparse(mt) / sqrt(V));
        rankd = intd(3, max(3, V / 6)), ranks = rankd(mt), m = min(8, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 3:
        V = distV(mt), p = min(1.0, sparse(mt) / sqrt(V));
        rankd = intd(5, min(V, 20)), ranks = rankd(mt), m = min(20, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 4:
        V = distV(mt), p = min(1.0, dense(mt));
        rankd = intd(3, max(3, V / 6)), ranks = rankd(mt), m = min(8, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 5:
        V = distV(mt), p = min(1.0, dense(mt));
        rankd = intd(5, min(V, 20)), ranks = rankd(mt), m = min(20, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 6:
        V = hugeV(mt), p = sparse(mt) / V;
        rankd = intd(3, max(3, V / 6)), ranks = rankd(mt), m = min(8, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 7:
        V = largeV(mt), p = sparse(mt) / V;
        rankd = intd(5, min(V, 20)), ranks = rankd(mt), m = min(20, V / ranks);
        g = random_uniform_level_flow(V, p, ranks, m);
        break;
    case 8:
        V = completeV(mt);
        g = complete_directed(V);
        break;
    case 9:
        X = gridV(mt), Y = gridV(mt), Z = gridV(mt), V = X * Y * Z;
        g = grid3_directed(X, Y, Z);
        break;
    }
    int E = g.size();
    caps = int_gen<long>(E, 1, max_cap);
    costs = int_gen<long>(E, 1, max_cost);
    return V;
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
