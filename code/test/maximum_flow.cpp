#include "../maximum_flow.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

// T = # tests, A = # test subjects, R = # random tests
constexpr int T = 5, A = 4, R = 100;
const char* names[T] = {"sparse flow networks", "dense flow networks",
                        "very dense flow networks", "large sparse flow networks",
                        "huge sparse flow networks"};
int quantity[T] = {1000, 500, 200, 100, 20};
bool hard[T] = {0, 0, 0, 1, 1};
vector<flow_graph> graphs[T];
vector<long> flows[A];

auto generate(int i) {
    intd distV(30, 200);
    intd largeV(2000, 4000);
    intd hugeV(10000, 50000);
    reald sparse(4.0, 12.0);
    int V;
    switch (i) {
    case 0:
        V = distV(mt);
        return random_flow_graph(V, sparse(mt) / V, 100'000'000);
    case 1:
        V = distV(mt);
        return random_flow_graph(V, 0.25, 100'000'000);
    case 2:
        V = distV(mt);
        return random_flow_graph(V, 1.0, 100'000'000);
    case 3:
        V = largeV(mt);
        return random_flow_graph(V, sparse(mt) / V, 100'000'000);
    case 4:
        V = hugeV(mt);
        return random_flow_graph(V, sparse(mt) / V, 100'000'000);
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

template <typename MF, int k>
void speed_test(const string& name, int t) {
    int G = graphs[t].size();
    flows[k].resize(G);

    auto now = steady_clock::now();
    for (int i = 0; i < G; i++) {
        const auto& f = graphs[t][i];
        int V = f.V;
        MF mf(V);
        for (int u = 0; u < V; u++) {
            for (int e : f.adj[u]) {
                mf.add(u, f.target[e], f.cap[e]);
            }
        }
        flows[k][i] = mf.maxflow(0, V - 1);
        if (hard[t])
            print("\r{} {}", i + 1, flows[k][i]);
    }
    auto time = duration_cast<ms>(steady_clock::now() - now);

    print("\r{}\r{}\t{}\n", string(50, ' '), time.count(), name.data());
}

void test_speed() {
    generate_randoms();
    for (int t = 0; t < T; t++) {
        print("--- {}  {}\n", t, names[t]);
        if (!hard[t])
            speed_test<edmonds_karp, 0>("edmonds karp", t);
        speed_test<dinitz_flow, 1>("dinitz flow", t);
        speed_test<push_relabel, 2>("push relabel", t);
        speed_test<tidal_flow, 3>("tidal flow", t);

        print("\n");
    }
}

void test_equal(int i) {
    intd distV(10, 20);
    int V = distV(mt);
    flow_graph f = random_flow_graph(V, 5.0 / V, 100000);

    edmonds_karp flow1(V);
    dinitz_flow flow2(V);
    push_relabel flow3(V);
    tidal_flow flow4(V);

    for (int u = 0; u < V; u++) {
        for (int e : f.adj[u]) {
            flow1.add(u, f.target[e], f.cap[e]);
            flow2.add(u, f.target[e], f.cap[e]);
            flow3.add(u, f.target[e], f.cap[e]);
            flow4.add(u, f.target[e], f.cap[e]);
        }
    }

    long mf1 = flow1.maxflow(0, V - 1);
    long mf2 = flow2.maxflow(0, V - 1);
    long mf3 = flow3.maxflow(0, V - 1);
    long mf4 = flow4.maxflow(0, V - 1);
    assert(mf1 != 0);

    string spaces(20, ' ');
    print("\rrandom test #{}: {} {} {} {}{}", i, mf1, mf2, mf3, mf4, spaces);
    if (!(mf1 == mf2 && mf2 == mf3 && mf3 == mf4)) {
        print("\nRandom test failed\n");
        exit(1);
    }
}

void test_equals() {
    for (int i = 1; i <= R; i++) {
        test_equal(i);
    }
    print("\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_equals();
    test_speed();
    return 0;
}
