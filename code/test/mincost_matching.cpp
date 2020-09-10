#include "../mincost_matching.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

// T = # tests, A = # test subjects, R = # random tests
constexpr int T = 4, A = 1, R = 300;
const char* names[T] = {"sparse", "dense", "complete", "sparse large"};
int quantity[T] = {200, 100, 50, 10};
bool hard[T] = {1, 1, 1, 1};
vector<cost_bipartite_graph> graphs[T];
vector<long> answers[A];

auto make_bipartite(int U, int V, int E, long max_weight) {
    auto g = random_exact_bipartite(U, V, E);
    return add_costs(g, max_weight);
}

auto make_bipartite(int U, int V, double p, long max_weight) {
    auto g = random_uniform_bipartite(U, V, p);
    return add_costs(g, max_weight);
}

auto generate(int i) {
    intd distV(30, 100);
    intd largeV(300, 500);
    reald sparse(10.0, 20.0);
    reald dense(0.3, 0.5);
    int V;
    switch (i) {
    case 0:
        V = distV(mt);
        return make_bipartite(V, V, sparse(mt) / V, 100'000'000);
    case 1:
        V = distV(mt);
        return make_bipartite(V, V, dense(mt), 100'000'000);
    case 2:
        V = distV(mt);
        return make_bipartite(V, V, 1.0, 100'000'000);
    case 3:
        V = largeV(mt);
        return make_bipartite(V, V, sparse(mt) / V, 100'000'000);
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

template <typename MCM>
MCM convert(const cost_bipartite_graph& g) {
    MCM mcm(g.U, g.V);
    for (int e = 0; e < g.E; e++) {
        int u = g.source[e], v = g.target[e], w = g.cost[e];
        mcm.add(u, v, w);
    }
    return mcm;
}

template <typename MCM, int k>
void speed_test(const string& name, int t) {
    int G = graphs[t].size();
    answers[k].resize(G);

    auto now = steady_clock::now();
    for (int i = 0; i < G; i++) {
        MCM mcm = convert<MCM>(graphs[t][i]);
        answers[k][i] = mcm.mincost_max_matching();
        if (hard[t])
            print("\r{} {}", i + 1, answers[k][i]);
    }
    auto time = duration_cast<ms>(steady_clock::now() - now);

    print("\r{}\r{}\t{}\n", string(50, ' '), time.count(), name.data());
}

void test_speed() {
    generate_randoms();
    for (int t = 0; t < T; t++) {
        print("--- {}  {}\n", t, names[t]);
        speed_test<mincost_hungarian, 1>("mincost hungarian", t);

        print("\n");
    }
}

// Unit tests
template <typename MCM>
void test() {
    MCM mm;
    int c;

    mm = MCM(2);
    mm.add(0, 0, 7);
    mm.add(0, 1, 6);
    mm.add(1, 0, 5);
    mm.add(1, 1, 2);
    c = mm.mincost_max_matching();
    print("min cost #1: {}\n", c);
    assert(c == 9);

    mm = MCM(3);
    mm.add(0, 0, 5);
    mm.add(0, 1, 4);
    mm.add(1, 0, 6);
    mm.add(1, 1, 3);
    mm.add(1, 2, 2);
    mm.add(2, 0, 1);
    mm.add(2, 2, 4);
    c = mm.mincost_max_matching();
    print("min cost #2: {}\n", c);
    assert(c == 7);

    mm = MCM(3);
    mm.add(0, 0, 108);
    mm.add(0, 1, 125);
    mm.add(0, 2, 150);
    mm.add(1, 0, 150);
    mm.add(1, 1, 135);
    mm.add(1, 2, 175);
    mm.add(2, 0, 122);
    mm.add(2, 1, 148);
    mm.add(2, 2, 250);
    c = mm.mincost_max_matching();
    print("min cost #3: {}\n", c);
    assert(c == 407);
}

// Random tests
void test_random(int R) {
    intd distV(40, 60);
    reald moderate(0.3, 0.6);

    for (int i = 1; i <= R; i++) {
        int U = distV(mt), V = distV(mt);
        auto g = make_bipartite(U, V, moderate(mt), 10000);
        auto mch = convert<mincost_hungarian>(g);
        long c = mch.mincost_max_matching();
        print("\rRandom {}... {}", i, c);
    }
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test<mincost_hungarian>();
    test_random(1000);
    test_speed();
    return 0;
}
