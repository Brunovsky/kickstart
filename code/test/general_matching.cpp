#include "../general_matching.hpp"

#include <fmt/format.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../graph_formats.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;
using fmt::format, fmt::print;

// *****

const string UNIT_TESTS = "datasets/micali_vazirani.txt";
const string ERROR_FILE = "datasets/latest_error.txt";

struct Test {
    string name, comment;
    micali_vazirani g;
    int M;
};

string apply_comment(string lines) {
    stringstream ss(lines);
    string commented, line;
    while (getline(ss, line)) {
        commented += "# " + line + "\n";
    }
    return commented;
}

[[noreturn]] void logerror(graph& g, micali_vazirani& vg, int M) {
    ofstream out(ERROR_FILE);

    vector<pair<int, int>> mates;
    for (int e = 0; e < g.E; e++)
        if (vg.mate[vg.source[e]] == e)
            mates.push_back({vg.source[e], vg.target[e]});

    int c = mates.size();
    out << apply_comment(to_dot(g));
    out << "\nRandom test error\n";
    out << to_simple(g, format("{} {}", c, M));
    for (auto [u, v] : mates)
        out << ' ' << u << ',' << v;
    out << endl;
    out.close();
    exit(0);
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
    boost::edmonds_maximum_cardinality_matching(bg, &mate[0]);
    int cnt = 0;
    for (auto& mapped : mate)
        cnt += mapped != bgraph::null_vertex();
    return cnt / 2;
}

int vg_matching_size(graph& g, micali_vazirani& vg, int M) {
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
        int e = g.edge.at({u, v});
        assert(g.mate[u] == -1 && g.mate[v] == -1);
        g.mate[u] = g.mate[v] = e;
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
    assert(matched == test.M);
    print("{:4} -- {:4} {}\n", matched, test.M, test.name);
}

void run_dataset_tests() {
    vector<Test> tests;
    ifstream file(UNIT_TESTS);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test);
}

void random_test(int R) {
    intd distV(18, 50);
    reald distE(1.2, 3.0);
    unordered_map<int, int> misscnt;

    for (int i = 1; i <= R; i++) {
        int V = distV(mt), E = int(V * distE(mt));
        auto g = random_exact_undirected_connected(V, E);
        g = relabel(g);
        shuffle_adj(g);
        bgraph bg = to_boost(g);
        micali_vazirani vg = to_mv(g);
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
    print("x{:<6}  V={:<6}  E={:<6}\n", R, V, E);

    vector<graph> gs(R);
    for (int i = 0; i < R; i++) {
        gs[i] = relabel(random_exact_undirected_connected(V, E));
        shuffle_adj(gs[i]);
    }

    auto now = steady_clock::now();
    for (int i = 0; i < R; i++) {
        auto vg = to_mv(gs[i]);
        vg.bootstrap();
        vg.max_matching();
    }
    auto time = duration_cast<milliseconds>(steady_clock::now() - now).count();

    print("\r time: {}ms\n", time);

    double ratio = 1e6 * time / (1.0 * R * E * sqrt(V));
    print("ratio: {:.2f}\n\n", ratio);
}

void scaling_tests(int M = 1) {
    scaling_test(M * 2000, 200, 300);
    scaling_test(M * 600, 200, 2000);
    scaling_test(M * 800, 500, 800);
    scaling_test(M * 100, 500, 12000);
    scaling_test(M * 80, 5000, 7000);
    scaling_test(M * 40, 5000, 12000);
    scaling_test(M * 10, 5000, 70000);
    scaling_test(M * 3, 5000, 230000);
    scaling_test(M * 30, 10000, 15000);
    scaling_test(M * 24, 10000, 25000);
    scaling_test(M * 5, 10000, 150000);
    scaling_test(M * 14, 20000, 30000);
    scaling_test(M * 9, 20000, 45000);
    scaling_test(M * 2, 20000, 400000);
    scaling_test(M * 6, 30000, 50000);
    scaling_test(M * 5, 30000, 70000);
    scaling_test(M * 1, 30000, 550000);
    scaling_test(M * 2, 50000, 80000);
    scaling_test(M * 1, 50000, 780000);
    scaling_test(M * 1, 100000, 150000);
    scaling_test(M * 1, 100000, 1'000'000);
}

void performance_test(int R, int V, int E) {
    vector<int> bans(R), vans(R);
    vector<graph> gs(R);
    int errors = 0;

    for (int i = 0; i < R; i++) {
        gs[i] = random_exact_undirected_connected(V, E);
        print("\rGenerating {}...", i + 1);
    }
    print("\n");

    // boost
    auto boost_now = steady_clock::now();
    for (int i = 0; i < R; i++) {
        auto bg = to_boost(gs[i]);
        bans[i] = boost_matching_size(bg);
        print("\rboost {}", i + 1);
    }
    auto boost_time = duration_cast<milliseconds>(steady_clock::now() - boost_now);
    print("\nboost time: {}ms\n", boost_time.count());

    // mv
    auto mv_now = steady_clock::now();
    for (int i = 0; i < R; i++) {
        auto vg = to_mv(gs[i]);
        vg.bootstrap();
        vans[i] = vg.max_matching();
        print("\r   mv {}", i + 1);
        errors += vans[i] != bans[i];
    }
    auto mv_time = duration_cast<milliseconds>(steady_clock::now() - mv_now);
    print("\n   mv time: {}ms\n", mv_time.count());
    print("errors: {}\n", errors);

    double ratio = 1e6 * mv_time.count() / (1.0 * R * E * sqrt(V));
    print("ratio: {:.2f}\n", ratio);
}

void performance_tests(int M) {
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
    // random_test(500000);
    run_dataset_tests();
    scaling_tests(1);
    // performance_tests(1);
    return 0;
}
