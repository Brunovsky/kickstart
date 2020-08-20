#include "../graph_generator.hpp"
#include "../maximum_flow.hpp"

using namespace std::chrono;
using ms = chrono::milliseconds;

// *****

// T = # tests, D = # dag tests only, A = # test subjects, R = # random tests
constexpr int T = 8, D = 4, A = 4, R = 2000;
string names[T] = {"sparse dags",        "dense dags",         "very dense dags",
                   "huge sparse dags",   "sparse general",     "dense general",
                   "very dense general", "huge sparse general"};
int quantity[T] = {1000, 500, 200, 60, 1000, 500, 200, 60};
vector<flow_graph> graphs[T];
vector<long> flows[A];

void generate_randoms() {
    intd distV(10, 200);
    intd hugeV(900, 2000);
    reald sparse(3.0, 7.0);
    for (int t = 0; t < quantity[0]; t++) {
        printf("\rGenerating sparse dag %d...", t + 1);
        int V = distV(mt);
        graphs[0].push_back(generate_dag_flow_graph(V, sparse(mt) / V, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[1]; t++) {
        printf("\rGenerating dense dag %d...", t + 1);
        int V = distV(mt);
        graphs[1].push_back(generate_dag_flow_graph(V, 0.2, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[2]; t++) {
        printf("\rGenerating very dense dag %d...", t + 1);
        int V = distV(mt);
        graphs[2].push_back(generate_dag_flow_graph(V, 1.0, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[3]; t++) {
        printf("\rGenerating sparse huge dag %d... ", t + 1);
        int V = hugeV(mt);
        graphs[3].push_back(generate_dag_flow_graph(V, sparse(mt) / V, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[4]; t++) {
        printf("\rGenerating sparse directed %d...", t + 1);
        int V = distV(mt);
        graphs[4].push_back(generate_flow_graph(V, sparse(mt) / V, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[5]; t++) {
        printf("\rGenerating dense directed %d... ", t + 1);
        int V = distV(mt);
        graphs[5].push_back(generate_flow_graph(V, 0.2, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[6]; t++) {
        printf("\rGenerating very dense directed %d... ", t + 1);
        int V = distV(mt);
        graphs[6].push_back(generate_flow_graph(V, 1.0, 100000));
    }
    printf(" OK\n");
    for (int t = 0; t < quantity[7]; t++) {
        printf("\rGenerating sparse huge directed %d... ", t + 1);
        int V = hugeV(mt);
        graphs[7].push_back(generate_flow_graph(V, sparse(mt) / V, 100000));
    }
    printf(" OK\n");
}

template <typename MF, int k>
void speed_test(const string& name, int t) {
    int G = graphs[t].size();

    auto now = chrono::steady_clock::now();
    for (int i = 0; i < G; i++) {
        const auto& f = graphs[t][i];
        int V = f.V, E = f.E;
        printf("\r\t%3d/%-3d\t\tV=%-5d  E=%-6d    ", i + 1, G, V, E);
        MF mf(V);
        for (int u = 0; u < V; u++) {
            for (int e : f.adj[u]) {
                mf.add(u, f.target[e], f.cap[e]);
            }
        }
        long max_flow = mf.maxflow(0, V - 1);
        flows[k].push_back(max_flow);
        printf("%ld\r", max_flow);
    }
    auto time = duration_cast<ms>(chrono::steady_clock::now() - now);

    printf("\n%lu\t%s\n", time.count(), name.data());
}

void test_speed(int S = T) {
    generate_randoms();
    for (int t = 0; t < S; t++) {
        printf("--- %d  %s\n", t, names[t].data());
        speed_test<edmonds_karp, 0>("edmonds karp", t);
        speed_test<dinitz_flow, 1>("dinitz", t);
        speed_test<push_relabel, 2>("push relabel", t);
        speed_test<tidal_flow, 3>("tidal flow", t);
        printf("\n");
    }
}

void test_equal() {
    intd distV(10, 40);
    int V = distV(mt);
    flow_graph f = generate_dag_flow_graph(V, 5.0 / V, 100000);

    tidal_flow flow1(V);
    edmonds_karp flow2(V);
    dinitz_flow flow3(V);
    push_relabel flow4(V);

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

    printf("%7ld %7ld %7ld %7ld", mf1, mf2, mf3, mf4);
    if (!(mf1 == mf2 && mf2 == mf3 && mf3 == mf4)) {
        printf("\nRandom test failed\n");
        exit(1);
    }
    assert(mf4 != 0);
}

void test_equals() {
    for (int i = 1; i <= R; i++) {
        printf("\rrandom test #%d: ", i);
        test_equal();
    }
    printf("\n");
}

int main() {
    setbuf(stdout, nullptr);
    test_equals();
    test_speed(T);
    (void)T, (void)D;
    return 0;
}
