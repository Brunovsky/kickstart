#include "../general_matching.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../debug_print.hpp"
#include "../graph_formats.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

#pragma clang diagnostic ignored "-Wfloat-conversion"

// *****

const string UNIT_TESTS = "datasets/micali_vazirani.txt";
const string ERROR_FILE = "datasets/latest_error.txt";

/*

struct Test {
    string name, comment;
    micali_vazirani g;
    int M;
};

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
        in >> u >> ws >> c >> ws >> v;
        g.add(u, v);
    }
    for (int i = 0; i < I; i++) {
        int u, v;
        char c;
        in >> u >> ws >> c >> ws >> v;
        assert(g.mate[u] == -1 && g.mate[v] == -1);
        g.mate[u] = v, g.mate[v] = u;
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
    print("{}", test.comment);
    int matched = test.g.max_matching();
    print("{:4} -- {:4} {}\n", matched, test.M, test.name);
    assert(matched == test.M);
}

void run_dataset_tests() {
    vector<Test> tests;
    ifstream file(UNIT_TESTS);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test);
}
*/

string apply_comment(string lines) {
    stringstream ss(lines);
    string commented, line;
    while (getline(ss, line)) {
        commented += "# " + line + "\n";
    }
    return commented;
}

[[noreturn]] void logerror(const edges_t& g, micali_vazirani& vg, int M) {
    ofstream out(ERROR_FILE);

    vector<pair<int, int>> mates;
    for (int u = 0; u < vg.V; u++)
        if (u < vg.mate[u])
            mates.push_back({u, vg.mate[u]});

    int c = mates.size();
    out << apply_comment(compact_dot(g, vg.V));
    out << "\nRandom test error\n";
    out << to_simple(g, vg.V, format("{} {}", c, M));
    for (auto [u, v] : mates)
        out << ' ' << u << ',' << v;
    out << endl;
    out.close();
    exit(0);
}

int vg_matching_size(const edges_t& g, micali_vazirani& vg, int M) {
    int ans = vg.max_matching();
    if (ans != M) {
        print("Bad matching size\n");
        logerror(g, vg, M);
    }
    return ans;
}

int vg_matching_size(micali_vazirani& vg) {
    try {
        return vg.max_matching();
    } catch (string error) {
        print("\r Error: {}\n", error);
        exit(0);
    }
}

bgraph to_boost(const edges_t& g, int V) {
    bgraph bg(V);
    for (auto [u, v] : g)
        add_edge(u, v, bg);
    return bg;
}

int boost_matching_size(const bgraph& bg) {
    matemap_t mate(num_vertices(bg));
    boost::edmonds_maximum_cardinality_matching(bg, &mate[0]);
    int cnt = 0;
    for (auto& mapped : mate)
        cnt += mapped != bgraph::null_vertex();
    return cnt / 2;
}

void random_test(int R) {
    intd distV(18, 30);
    reald distE(1.2, 3.0);
    unordered_map<int, int> misscnt;

    for (int i = 1; i <= R; i++) {
        int V = distV(mt), E = int(V * distE(mt));
        auto g = relabel(random_exact_undirected_connected(V, E), V);
        shuffle(begin(g), end(g), mt);
        bgraph bg = to_boost(g, V);
        micali_vazirani vg(V, g);
        int M = boost_matching_size(bg);
        int ans = vg_matching_size(g, vg, M);
        int missed = V / 2 - M;
        misscnt[missed]++;
        print("\rRandom test {}... ", i);
        if (ans != M) {
            print("ERROR\n");
            logerror(g, vg, M);
        }
    }
}

void scaling_test(int R, int V, int E) {
    if (R == 0)
        return;

    print("x{:<6}  V={:<6}  E={:<6}\n", R, V, E);

    vector<edges_t> gs(R);
    for (int i = 0; i < R; i++) {
        gs[i] = relabel(random_exact_undirected_connected(V, E), V);
    }

    START(run);
    for (int i = 0; i < R; i++) {
        micali_vazirani vg(V, gs[i]);
        vg.bootstrap();
        vg.max_matching();
    }
    TIME(run);
    PRINT(run);

    double ratio = 1e6 * time_run / (1.0 * R * E * sqrt(V));
    print("ratio: {:.2f}\n", ratio);
}

void scaling_tests(double M = 1) {
    START(scaling);
    scaling_test(M * 2000, 200, 300);
    scaling_test(M * 600, 200, 2000);
    scaling_test(M * 800, 500, 800);
    scaling_test(M * 100, 500, 12000);
    scaling_test(M * 80, 5000, 7000);
    scaling_test(M * 40, 5000, 12000);
    scaling_test(M * 10, 5000, 70000);
    scaling_test(M * 3, 5000, 230'000);
    scaling_test(M * 30, 10000, 15000);
    scaling_test(M * 24, 10000, 25000);
    scaling_test(M * 5, 10000, 150'000);
    scaling_test(M * 14, 20000, 30000);
    scaling_test(M * 9, 20000, 45000);
    scaling_test(M * 2, 20000, 400'000);
    scaling_test(M * 6, 30000, 50000);
    scaling_test(M * 5, 30000, 70000);
    scaling_test(M * 1, 30000, 550'000);
    scaling_test(M * 4, 50000, 80000);
    scaling_test(M * 1, 50000, 780'000);
    scaling_test(M * 3, 100000, 150'000);
    scaling_test(M * 2, 100000, 250'000);
    scaling_test(M * 2, 100000, 350'000);
    scaling_test(M * 1, 100000, 1'000'000);
    scaling_test(M * 2, 200000, 300'000);
    scaling_test(M * 2, 250000, 300'000);
    TIME(scaling);
    PRINT(scaling);
}

void performance_test(int R, int V, int E) {
    if (R == 0)
        return;

    print("x{:>6}  V={:<6}  E={:<6}\n", R, V, E);

    vector<int> bans(R), vans(R);
    vector<edges_t> gs(R);
    for (int i = 0; i < R; i++) {
        gs[i] = relabel(random_exact_undirected_connected(V, E), V);
        print("\rGenerating {}...", i + 1);
    }
    print("\n");

    // boost
    START(boost);
    for (int i = 0; i < R; i++) {
        auto bg = to_boost(gs[i], V);
        bans[i] = boost_matching_size(bg);
        print("\rboost {}", i + 1);
    }
    TIME(boost);
    PRINT(boost);

    // mv
    int errors = 0;
    START(mv);
    for (int i = 0; i < R; i++) {
        micali_vazirani vg(V, gs[i]);
        vg.bootstrap();
        vans[i] = vg.max_matching();
        errors += vans[i] != bans[i];
    }
    TIME(mv);
    PRINT(mv);
    print("errors: {}\n", errors);

    double ratio = 1e6 * time_mv / (1.0 * R * E * sqrt(V));
    print("ratio: {:.2f}\n", ratio);
}

void performance_tests(double M) {
    performance_test(M * 20000, 50, 70);
    performance_test(M * 10000, 100, 150);
    performance_test(M * 4000, 200, 300);
    performance_test(M * 2000, 500, 800);
    performance_test(M * 200, 5000, 7000);
    performance_test(M * 140, 5000, 12000);
    performance_test(M * 40, 10000, 15000);
    performance_test(M * 30, 10000, 25000);
    performance_test(M * 14, 20000, 30000);
    performance_test(M * 11, 20000, 45000);
    performance_test(M * 6, 30000, 50000);
    performance_test(M * 5, 30000, 70000);
    performance_test(M * 2, 50000, 80000);
    performance_test(M * 1, 100000, 150000);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    // run_dataset_tests();
    random_test(10000);
    // scaling_tests(3);
    performance_tests(1);
    return 0;
}
