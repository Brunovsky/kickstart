#include <fmt/format.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../general_matching.hpp"
#include "../graph_formats.hpp"
#include "../graph_generator.hpp"

using namespace std::chrono;
using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;
using fmt::format, fmt::print;

// *****

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
    return cnt / 2;
}

string apply_comment(string lines) {
    stringstream ss(lines);
    string commented, line;
    while (getline(ss, line)) {
        commented += "# " + line + "\n";
    }
    return commented;
}

[[noreturn]] void logerror(graph& g, micali_vazirani& vg, int M) {
    ofstream out("datasets/latest_error.txt");
    vector<int> matched;

    for (int e = 0; e < g.E; e++)
        if (vg.mate[vg.source[e]] == e)
            matched.push_back(e);

    int c = matched.size();
    out << apply_comment(to_dot(g));
    out << "\nRandom test error\n";
    out << to_simple(g, format("{} {}", c, M));
    for (int e : matched)
        out << ' ' << e;
    out << endl;
    out.close();
    exit(0);
}

int vg_matching_size(graph& g, micali_vazirani& vg, int M) {
    try {
        int ans = vg.max_matching();
        if (ans != M) {
            throw "Bad matching size"s;
        }
        return ans;
    } catch (string error) {
        print("\r Error: {}\n", error);
        logerror(errorfile, g, vg, M);
    }
}

int vg_matching_size(micali_vazirani& vg) {
    try {
        return vg.max_matching();
    } catch (string error) {
        print("\r Error: {}\n", error);
        exit(0);
    }
}

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
    debug_header(test.name);
    dprint("{}", test.comment);
    int matched = vg_matching_size(test.g);
    print("{:4} -- {:4} {}\n", matched, test.M, test.name);
}

void run_dataset_tests(string filename) {
    vector<Test> tests;
    ifstream file(filename);
    assert(file.is_open());
    read_unit_tests(tests, file);
    for_each(begin(tests), end(tests), run_test);
}

void random_test(int R = 1000000) {
    intd distV(18, 50);
    reald distE(1.2, 3.0);
    unordered_map<int, int> misscnt;

    for (int i = 1; i <= R; i++) {
        int V = distV(mt), E = int(V * distE(mt));
        auto g = random_exact_undirected_connected(V, E);
        g = relabel(g); // randomize the tree structure
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
    print("errors: {}\n", errors);

    double ratio = 1e9 * mv_time.count() / (R * E * sqrt(V));
    print("ratio: {}\n", ratio);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    // run_dataset_tests("datasets/micali_vazirani.txt");
    performance_test(10000, 500, 800);
    performance_test(1000, 5000, 7000);
    performance_test(700, 5000, 12000);
    performance_test(200, 10000, 15000);
    performance_test(150, 10000, 25000);
    performance_test(70, 20000, 30000);
    performance_test(55, 20000, 45000);
    performance_test(30, 30000, 50000);
    performance_test(25, 30000, 70000);
    performance_test(10, 50000, 80000);
    performance_test(5, 100000, 150000);
    random_test();
    return 0;
}
