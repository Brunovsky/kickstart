#include "../general_matching.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../debug_print.hpp"
#include "../graph_formats.hpp"
#include "../graph_generator.hpp"
#include "test_utils.hpp"

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

// #pragma clang diagnostic ignored "-Wfloat-conversion"

// *****

const string DATASET_FILE = "datasets/micali_vazirani.txt";
const string ERROR_FILE = "datasets/latest_error.txt";

struct general_matching_dataset_test_t {
    string name, comment;
    edges_t g;
    vector<int> mate;
    int V, E, num_mates, ans;

    void read(istream& in) {
        while (in.peek() == '#') {
            string line;
            getline(in, line);
            comment += line + "\n";
        }
        in >> ws, getline(in, name);
        in >> V >> E >> num_mates >> ans;
        assert(V > 0 && E > 0 && num_mates > 0 && ans >= 0 && !in.bad());
        assert(num_mates < 2 * V && ans < 2 * V && num_mates <= ans);

        g.resize(E), mate.assign(V, -1);
        for (int e = 0; e < E; e++) {
            char c;
            in >> g[e][0] >> ws >> c >> ws >> g[e][1];
        }
        for (int i = 0, u, v; i < num_mates; i++) {
            char c;
            in >> u >> ws >> c >> ws >> v;
            assert(mate[u] == -1 && mate[v] == -1), mate[u] = v, mate[v] = u;
        }
    }

    void run() const {
        micali_vazirani vg(V, g);
        vg.mate = mate;
        int m = vg.max_matching();
        print("{:4} -- {:4} {}\n", m, ans, name);
        assert(m == ans);
    }
};

void dataset_test_general_matching() {
    ifstream file(DATASET_FILE);
    if (!file.is_open())
        return print("dataset file {} not found, skipping dataset test\n", DATASET_FILE);
    file >> ws;
    while (!file.eof()) {
        general_matching_dataset_test_t test;
        test.read(file);
        test.run();
        file >> ws;
    }
    print_ok("dataset test general matching");
}

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
    out << apply_comment(compact_dot(g, vg.V)) << '\n';
    out << "Random test error" << '\n';
    out << to_simple(g, vg.V, format("{} {}", c, M));
    for (auto [u, v] : mates)
        out << ' ' << u << ',' << v;
    out << '\n';
    out.close();
    print("Logged error case to {}\n", ERROR_FILE);
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

void stress_test_general_matching(int T = 10000) {
    intd distV(18, 30);
    reald distE(1.2, 3.0);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "random stress test");
        int V = distV(mt), E = int(V * distE(mt));
        auto g = relabel(random_exact_undirected_connected(V, E), V);
        shuffle(begin(g), end(g), mt);
        bgraph bg = to_boost(g, V);
        micali_vazirani vg(V, g);
        int M = boost_matching_size(bg);
        int ans = vg_matching_size(g, vg, M);
        if (ans != M) {
            print("error -- V={} E={} -- boost:{} -- mv:{}\n", V, E, M, ans);
            logerror(g, vg, M);
        }
    }
}

inline double compute_ratio(int64_t time, int T, int V, int E) {
    return 1e9 * time / (1.0 * T * E * sqrt(V));
}

void scaling_test_general_matching_run(int T, int V, int E) {
    if (T == 0)
        return;

    START_ACC(mv);
    for (int i = 0; i < T; i++) {
        edges_t g = relabel(random_exact_undirected_connected(V, E), V);
        START(mv);
        micali_vazirani vg(V, g);
        vg.bootstrap();
        vg.max_matching();
        ADD_TIME(mv);
    }
    print(" {:>8}ms -- {:7.1f} ratio -- x{:<6}  V={:<6}  E={:<6}\n", TIME_MS(mv),
          compute_ratio(TIME_MS(mv), T, V, E), T, V, E);
}

void scaling_test_general_matching(double F = 1.0) {
    print("scaling test general matching (factor {})\n", F);
    scaling_test_general_matching_run(int(F * 2000), 200, 300);
    scaling_test_general_matching_run(int(F * 600), 200, 2000);
    scaling_test_general_matching_run(int(F * 800), 500, 800);
    scaling_test_general_matching_run(int(F * 100), 500, 12000);
    scaling_test_general_matching_run(int(F * 80), 5000, 7000);
    scaling_test_general_matching_run(int(F * 40), 5000, 12000);
    scaling_test_general_matching_run(int(F * 10), 5000, 70000);
    scaling_test_general_matching_run(int(F * 3), 5000, 230'000);
    scaling_test_general_matching_run(int(F * 30), 10000, 15000);
    scaling_test_general_matching_run(int(F * 24), 10000, 25000);
    scaling_test_general_matching_run(int(F * 5), 10000, 150'000);
    scaling_test_general_matching_run(int(F * 14), 20000, 30000);
    scaling_test_general_matching_run(int(F * 9), 20000, 45000);
    scaling_test_general_matching_run(int(F * 2), 20000, 400'000);
    scaling_test_general_matching_run(int(F * 6), 30000, 50000);
    scaling_test_general_matching_run(int(F * 5), 30000, 70000);
    scaling_test_general_matching_run(int(F * 1), 30000, 550'000);
    scaling_test_general_matching_run(int(F * 4), 50000, 80000);
    scaling_test_general_matching_run(int(F * 1), 50000, 780'000);
    scaling_test_general_matching_run(int(F * 3), 100000, 150'000);
    scaling_test_general_matching_run(int(F * 2), 100000, 250'000);
    scaling_test_general_matching_run(int(F * 2), 100000, 350'000);
    scaling_test_general_matching_run(int(F * 1), 100000, 1'000'000);
    scaling_test_general_matching_run(int(F * 2), 200000, 300'000);
    scaling_test_general_matching_run(int(F * 2), 250000, 300'000);
}

void speed_test_general_matching_run(int T, int V, int E) {
    print("  speed test x{:>6}  V={:<6}  E={:<6}\n", T, V, E);
    START_ACC(boost);
    START_ACC(mv);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "speed test general matching");
        edges_t g = relabel(random_exact_undirected_connected(V, E), V);
        auto bg = to_boost(g, V);

        START(boost);
        int ans = boost_matching_size(bg);
        ADD_TIME(boost);

        START(mv);
        micali_vazirani vg(V, g);
        vg.bootstrap();
        int M = vg.max_matching();
        ADD_TIME(mv);

        if (ans != M) {
            print("error -- V={} E={} -- boost:{} -- mv:{}\n", V, E, M, ans);
        }
    }

    PRINT_TIME(boost);
    PRINT_TIME(mv);
}

void speed_test_general_matching(double F = 1.0) {
    print("speed test general matching (factor {})\n", F);
    speed_test_general_matching_run(int(F * 20000), 50, 70);
    speed_test_general_matching_run(int(F * 10000), 100, 150);
    speed_test_general_matching_run(int(F * 4000), 200, 300);
    speed_test_general_matching_run(int(F * 2000), 500, 800);
    speed_test_general_matching_run(int(F * 200), 5000, 7000);
    speed_test_general_matching_run(int(F * 140), 5000, 12000);
    speed_test_general_matching_run(int(F * 40), 10000, 15000);
    speed_test_general_matching_run(int(F * 30), 10000, 25000);
    speed_test_general_matching_run(int(F * 14), 20000, 30000);
    speed_test_general_matching_run(int(F * 11), 20000, 45000);
    speed_test_general_matching_run(int(F * 6), 30000, 50000);
    speed_test_general_matching_run(int(F * 5), 30000, 70000);
    speed_test_general_matching_run(int(F * 2), 50000, 80000);
    speed_test_general_matching_run(int(F * 1), 100000, 150000);
}

int main() {
    dataset_test_general_matching();
    stress_test_general_matching();
    scaling_test_general_matching(3);
    speed_test_general_matching();
    return 0;
}
