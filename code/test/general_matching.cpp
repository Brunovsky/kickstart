#include "../generators/general_matching.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../formatting.hpp"
#include "../generators/graph_formats.hpp"
#include "../matching/micali_vazirani.hpp"
#include "test_utils.hpp"

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

// *****

const string DATASET_FILE = "datasets/micali_vazirani.txt";
const string ERROR_FILE = "datasets/latest_error.txt";

bgraph to_boost(int V, const edges_t& g) {
    bgraph bg(V);
    for (auto [u, v] : g)
        add_edge(u, v, bg);
    return bg;
}

int boost_matching_size(const bgraph& bg) {
    matemap_t mate(num_vertices(bg));
    boost::greedy_matching<bgraph, size_t*>::find_matching(bg, &mate[0]);
    boost::edmonds_maximum_cardinality_matching(bg, &mate[0]);
    int cnt = 0;
    for (auto& mapped : mate)
        cnt += mapped != bgraph::null_vertex();
    return cnt / 2;
}

inline double compute_ratio(int64_t time, int T, int V, int E) {
    return 1e9 * time / (1.0 * T * E * sqrt(V));
}

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

void stress_test_general_matching(int T = 10000) {
    intd distV(30, 100);
    reald distMp(0.1, 0.5);
    reald distEp(3.0, 15.0);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "stress test general matching");
        int V = distV(mt);
        int M = min(V / 2, int(ceil(distMp(mt) * V)));
        double p = distEp(mt) / V;
        auto g = random_general_matching(V, M, p);
        general_matching_hide_topology(V, g);

        bgraph bg = to_boost(V, g);
        int boo = boost_matching_size(bg);

        micali_vazirani vg(V, g);
        vg.bootstrap();
        int ans = vg.max_matching();

        if (M != boo || M != ans) {
            int E = g.size();
            fail("error -- V={} E={} M={} -- boost:{} -- mv:{}\n", V, E, M, boo, ans);
        }
    }

    print_ok("stress test general matching");
}

void scaling_test_general_matching_run(int T, int V, int E) {
    if (T == 0)
        return;

    START_ACC(mv);
    for (int t = 0; t < T; t++) {
        print_progress(t, T, "scaling test");
        edges_t g = relabel(V, random_exact_undirected_connected(V, E));

        START(mv);
        micali_vazirani vg(V, g);
        vg.bootstrap();
        vg.max_matching();
        ADD_TIME(mv);
    }

    print(" {:>8}ms -- {:>7.1f}ms each -- {:7.1f} ratio -- x{:<6}  V={:<6}  E={:<6}\n",
          TIME_MS(mv), 1.0 * TIME_MS(mv) / T, compute_ratio(TIME_MS(mv), T, V, E), T, V,
          E);
}

void scaling_test_general_matching(double F = 1.0) {
    print("scaling test general matching (factor {})\n", F);
    scaling_test_general_matching_run(int(F * 2000), 200, 300);
    scaling_test_general_matching_run(int(F * 600), 200, 2'000);
    scaling_test_general_matching_run(int(F * 800), 500, 800);
    scaling_test_general_matching_run(int(F * 100), 500, 12'000);
    scaling_test_general_matching_run(int(F * 300), 1'000, 2'000);
    scaling_test_general_matching_run(int(F * 200), 1'000, 8'000);
    scaling_test_general_matching_run(int(F * 100), 1'000, 30'000);
    scaling_test_general_matching_run(int(F * 80), 5'000, 7'000);
    scaling_test_general_matching_run(int(F * 40), 5'000, 16'000);
    scaling_test_general_matching_run(int(F * 10), 5'000, 70'000);
    scaling_test_general_matching_run(int(F * 3), 5'000, 230'000);
    scaling_test_general_matching_run(int(F * 30), 10'000, 15'000);
    scaling_test_general_matching_run(int(F * 24), 10'000, 25'000);
    scaling_test_general_matching_run(int(F * 5), 10'000, 150'000);
    scaling_test_general_matching_run(int(F * 14), 20'000, 30'000);
    scaling_test_general_matching_run(int(F * 9), 20'000, 45'000);
    scaling_test_general_matching_run(int(F * 2), 20'000, 400'000);
    scaling_test_general_matching_run(int(F * 6), 30'000, 50'000);
    scaling_test_general_matching_run(int(F * 5), 30'000, 70'000);
    scaling_test_general_matching_run(int(F * 1), 30'000, 550'000);
    scaling_test_general_matching_run(int(F * 4), 50'000, 80'000);
    scaling_test_general_matching_run(int(F * 1), 50'000, 780'000);
    scaling_test_general_matching_run(int(F * 3), 100'000, 150'000);
    scaling_test_general_matching_run(int(F * 2), 100'000, 250'000);
    scaling_test_general_matching_run(int(F * 2), 100'000, 350'000);
    scaling_test_general_matching_run(int(F * 1), 100'000, 1'000'000);
    scaling_test_general_matching_run(int(F * 2), 200'000, 300'000);
    scaling_test_general_matching_run(int(F * 2), 250'000, 300'000);
    print_ok("scaling test general matching");
}

void speed_test_general_matching_run(int T, int V, int E) {
    print("  speed test x{:<6}  V={:<6}  E={}\n", T, V, E);
    START_ACC(boost);
    START_ACC(mv);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "speed test general matching");
        edges_t g = relabel(V, random_exact_undirected_connected(V, E));
        auto bg = to_boost(V, g);

        START(boost);
        int ans = boost_matching_size(bg);
        ADD_TIME(boost);

        START(mv);
        micali_vazirani vg(V, g);
        vg.bootstrap();
        int M = vg.max_matching();
        ADD_TIME(mv);

        assert(ans == M);
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
    speed_test_general_matching_run(int(F * 700), 500, 3'000);
    speed_test_general_matching_run(int(F * 300), 500, 8'000);
    speed_test_general_matching_run(int(F * 300), 2'000, 5'000);
    speed_test_general_matching_run(int(F * 200), 2'000, 12'000);
    speed_test_general_matching_run(int(F * 200), 5'000, 7'000);
    speed_test_general_matching_run(int(F * 80), 5'000, 30'000);
    speed_test_general_matching_run(int(F * 40), 10'000, 15'000);
    speed_test_general_matching_run(int(F * 30), 10'000, 25'000);
    speed_test_general_matching_run(int(F * 14), 20'000, 30'000);
    speed_test_general_matching_run(int(F * 11), 20'000, 45'000);
    speed_test_general_matching_run(int(F * 6), 30'000, 50'000);
    speed_test_general_matching_run(int(F * 5), 30'000, 70'000);
    speed_test_general_matching_run(int(F * 2), 50'000, 80'000);
    speed_test_general_matching_run(int(F * 1), 100'000, 150'000);
    print_ok("speed test general matching");
}

int main() {
    dataset_test_general_matching();
    stress_test_general_matching();
    scaling_test_general_matching();
    speed_test_general_matching();
    return 0;
}
