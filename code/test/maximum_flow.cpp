#include "../maximum_flow.hpp"

#include "../debug_print.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

// T = # tests, A = # test subjects, R = # random tests
constexpr int T = 10, A = 4, R = 100;
const char* names[T] = {"sparse long level flow networks",
                        "sparse wide level flow networks",
                        "moderate long level flow networks",
                        "moderate wide level flow networks",
                        "dense long level flow networks",
                        "dense wide level flow networks",
                        "large sparse long flow networks",
                        "large sparse wide flow networks",
                        "complete flow networks",
                        "3d grid flow networks"};
int quantity[T] = {400, 400, 400, 400, 400, 400, 30, 30, 100, 500};
vector<edges_t> graphs;
vector<cap_t> caps;
vector<int> gV;
vector<long> flows[A];

auto random_cap(int E, long max_cap) {
    longd dist(1, max_cap);
    cap_t cap(E);
    for (int e = 0; e < E; e++)
        cap[e] = dist(mt);
    return cap;
}

int generate(int i, edges_t& g, cap_t& cap, long max_cap = 99) {
    intd smallV(30, 50);
    intd distV(900, 1300);
    intd largeV(2000, 3000);
    intd hugeV(8000, 11000);
    intd completeV(600, 900);
    intd gridV(10, 40);
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
    cap = random_cap(E, max_cap);
    return V;
}

void generate_randoms(int t, long max_cap = 100'000) {
    int Q = quantity[t];
    graphs.resize(Q);
    caps.resize(Q);
    gV.resize(Q);
    for (int i = 0; i < Q; i++) {
        print("\rGenerating {} {}...", names[t], i + 1);
        gV[i] = generate(t, graphs[i], caps[i], max_cap);
    }
    print("\n");
}

template <typename MF, int k>
void speed_test(const string& name) {
    int T = graphs.size();
    flows[k].resize(T);

    START(flow);
    for (int i = 0; i < T; i++) {
        int V = gV[i];
        const auto& g = graphs[i];
        const auto& cap = caps[i];
        MF mf(V, g, cap);
        flows[k][i] = mf.maxflow(0, V - 1);
        print("\r{:>4} {}", i + 1, flows[k][i]);
    }
    TIME(flow);
    CHRONO_PRINT_NAME(flow, name);
}

void test_speed() {
    for (int t = 0; t < T; t++) {
        generate_randoms(t);
        print("--- {}  {}\n", t, names[t]);
        speed_test<dinitz_flow, 1>("dinitz flow");
        speed_test<push_relabel, 2>("push relabel");
        // speed_test<tidal_flow, 3>("tidal flow", t);

        print("\n");
        if (flows[1] != flows[2]) {
            print("Different answers!\n");
            int Q = quantity[t], c = 0;
            for (int i = 0; i < Q && c < 10; i++) {
                if (flows[1][i] != flows[2][i]) {
                    print("dinitz: {:8} | ", flows[1][i]);
                    print("preflow: {:8}\n", flows[2][i]);
                    c++;
                }
            }
        }
    }
}

void test_equal(int i) {
    intd distV(10, 20);
    edges_t g;
    cap_t cap;
    int V = generate(-1, g, cap, 100'000);

    edmonds_karp flow1(V, g, cap);
    dinitz_flow flow2(V, g, cap);
    push_relabel flow3(V, g, cap);
    // tidal_flow flow4(V, g, cap);

    long mf1 = flow1.maxflow(0, V - 1);
    long mf2 = flow2.maxflow(0, V - 1);
    long mf3 = flow3.maxflow(0, V - 1);
    long mf4 = mf3; // flow4.maxflow(0, V - 1);
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
