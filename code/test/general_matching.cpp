#include "../general_matching.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../graph_generator.hpp"

using namespace std::chrono;
using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

// *****

void debug_header(string name) {
    dflash();
    dprint("{} {} {}\n", string(70, '='), name, string(70, '='));
    dflash();
}

struct Test {
    string name, comment;
    micali_vazirani g;
    int M;
};

Test read_unit_test(istream& in) {
    Test test;
    auto& g = test.g;
    while (in.peek() == '#') {
        string line;
        getline(in, line);
        test.comment += line + "\n";
    }
    in >> ws;
    getline(in, test.name);

    int V, E, I, M;
    in >> ws >> V >> E >> I >> M >> ws;
    assert(V >= 0 && E >= 0 && I >= 0 && M >= 0 && I < 2 * V && M < 2 * V && I <= M);
    assert(!in.bad());

    g = micali_vazirani(V);
    test.M = M;
    for (int i = 0; i < E; i++) {
        int u, v;
        char c;
        in >> u >> c >> v;
        g.add(u, v);
    }
    for (int i = 0; i < I; i++) {
        int e;
        in >> e;
        g.mate[g.source[e]] = g.mate[g.target[e]] = e;
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

void run_test(Test& test) {
    debug_header(test.name);
    dprint("{}", test.comment);
    int matched = test.g.max_matching();
    print("{:4} -- {:4} {}\n", matched, test.M, test.name);
    // assert(matched == test.M);
}

void run_dataset_tests(string filename) {
    vector<Test> tests;
    ifstream file(filename);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test);
}

micali_vazirani to_mv(const graph& g) {
    micali_vazirani vg(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            if (u < v)
                vg.add(u, v);
        }
    }
    return vg;
}

bgraph to_boost(const graph& g) {
    bgraph bg(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            add_edge(u, v, bg);
        }
    }
    return bg;
}

int boost_matching_size(const bgraph& bg) {
    matemap_t mate(num_vertices(bg));
    bool ok = boost::checked_edmonds_maximum_cardinality_matching(bg, &mate[0]);
    (void)ok, assert(ok);
    int cnt = 0;
    for (auto& mapped : mate)
        cnt += mapped != bgraph::null_vertex();
    return cnt;
}

void random_test(int R = 100, int step = 5) {
    intd distv(1000, 50000);
    reald sparse(1.0, 6.0);

    unordered_map<int, int> misscnt;
    int errors = 0;

    for (int i = 0; i < R; i++) {
        int V = distv(mt);
        auto g = generate_uniform_undirected(V, sparse(mt) / V);
        g = relabel(g); // randomize the tree structure
        shuffle_adj(g);
        bgraph bg = to_boost(g);
        micali_vazirani vg = to_mv(g);
        int bans = boost_matching_size(bg) / 2;
        int vans = vg.max_matching();
        int missed = V / 2 - bans;
        misscnt[missed]++;
        print("\r{:7} | {:6} | {:6}   {:4} {:5}", i + 1, vans, bans, g.V, g.E);
        if (vans != bans) {
            print("\n");
            errors++;
        }

        if (i % step == step - 1) {
            print("\nerrors={:<3}  miss counts: ", errors);
            for (int k = 0; k < 10; k++) {
                print(" {}->{:<6}", k, misscnt[k]);
            }
            print("\n");
        }
    }
}

// use boost's edmonds implementation for reference
void performance_test(int R = 100, int Vlo = 500, int Vhi = 20000) {
    intd distv(Vlo, Vhi);
    reald sparse(1.0, 6.0);

    vector<int> bans(R), vans(R);
    vector<graph> gs(R);

    mv_search_cnt = 0;
    mv_good_cnt = 0;
    int errors = 0;

    for (int i = 0; i < R; i++) {
        int V = distv(mt);
        gs[i] = generate_uniform_undirected(V, sparse(mt) / V);
        print("\rGenerating {}...", i + 1);
    }
    print("\n");

    // boost
    auto boost_now = steady_clock::now();
    for (int i = 0; i < R; i++) {
        auto bg = to_boost(gs[i]);
        bans[i] = boost_matching_size(bg) / 2;
        dprint("\rboost {}", i + 1);
    }
    auto boost_time = duration_cast<milliseconds>(steady_clock::now() - boost_now);
    print("\nboost time: {}ms\n", boost_time.count());

    // mv
    auto mv_now = steady_clock::now();
    for (int i = 0; i < R; i++) {
        auto vg = to_mv(gs[i]);
        vans[i] = vg.max_matching();
        dprint("\r   mv {}", i + 1);
        errors += vans[i] != bans[i];
    }
    auto mv_time = duration_cast<milliseconds>(steady_clock::now() - mv_now);
    print("\n   mv time: {}ms\n", mv_time.count());
    print("searches: {} (avg: {:.2})\n", mv_search_cnt, 1.0 * mv_search_cnt / R);
    print("good phases: {} (avg: {:.2})\n", mv_good_cnt, 1.0 * mv_good_cnt / R);
    print("errors : {}\n", errors);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    run_dataset_tests("datasets/micali_vazirani.txt");
    performance_test();
    random_test();
    return 0;
}
