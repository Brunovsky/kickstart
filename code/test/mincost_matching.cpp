#include "../mincost_matching.hpp"

#include "../graph_generator.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

// T = # tests, A = # test subjects, R = # random tests
constexpr int T = 4, A = 4, R = 300;
const char* names[T] = {"sparse", "dense", "complete", "sparse large"};
int quantity[T] = {200, 100, 50, 10};
bool hard[T] = {1, 1, 1, 1};
vector<weight_bipartite_graph> graphs[T];
vector<long> answers[A];

weight_bipartite_graph make_bipartite(int U, int V, int E, long max_weight) {
    bipartite_graph g = random_exact_bipartite(U, V, E);
    return add_weights(g, max_weight);
}

weight_bipartite_graph make_bipartite(int U, int V, double p, long max_weight) {
    bipartite_graph g = random_uniform_bipartite(U, V, p);
    return add_weights(g, max_weight);
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
MCM convert(const weight_bipartite_graph& g) {
    MCM mcm(g.V);
    for (int e = 0; e < g.E; e++) {
        int u = g.source[e], v = g.target[e], w = g.weight[e];
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
        answers[k][i] = mcm.compute();
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
        speed_test<mincost_perfect_matching, 0>("perfect hungarian", t);
        speed_test<mincost_maximum_matching, 1>("maximum hungarian", t);

        print("\n");
    }
}

// Unit tests
template <typename MM>
void test() {
    MM mm(2);
    mm.add(0, 0, 7);
    mm.add(0, 1, 6);
    mm.add(1, 0, 5);
    mm.add(1, 1, 2);
    int min_cost = mm.compute();
    printf("min cost #1: %d\n", min_cost);
    assert(min_cost == 9);

    mm = MM(3);
    mm.add(0, 0, 5);
    mm.add(0, 1, 4);
    mm.add(1, 0, 6);
    mm.add(1, 1, 3);
    mm.add(1, 2, 2);
    mm.add(2, 0, 1);
    mm.add(2, 2, 4);
    min_cost = mm.compute();
    printf("min cost #2: %d\n", min_cost);
    assert(min_cost == 7);

    mm = MM(3);
    mm.add(0, 0, 108);
    mm.add(0, 1, 125);
    mm.add(0, 2, 150);
    mm.add(1, 0, 150);
    mm.add(1, 1, 135);
    mm.add(1, 2, 175);
    mm.add(2, 0, 122);
    mm.add(2, 1, 148);
    mm.add(2, 2, 250);
    min_cost = mm.compute();
    printf("min cost #3: %d\n", min_cost);
    assert(min_cost == 407);
}

// Random tests
void test_random(int R) {
    mt.seed(73);
    intd distV(100, 200);
    reald moderate(0.3, 0.6);

    for (int i = 1; i <= R; i++) {
        int V = distV(mt);
        auto g = make_bipartite(V, V, moderate(mt), 10000);
        auto mpm = convert<mincost_perfect_matching>(g);
        auto mmm = convert<mincost_maximum_matching>(g);
        long c1 = mpm.compute();
        long c2 = mmm.compute();
        print("c1: {:<10}  c2: {:<10}\n", c1, c2);
    }
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test<mincost_perfect_matching>();
    test<mincost_maximum_matching>();
    test_random(30);
    test_speed();
    return 0;
}
