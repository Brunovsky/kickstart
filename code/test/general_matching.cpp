#include "test_utils.hpp"
#include "../lib/general_matching.hpp"
#include "../lib/graph_formats.hpp"
#include "../matching/micali_vazirani.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

const string DATASET_FILE = "datasets/micali_vazirani.txt";
const string ERROR_FILE = "datasets/latest_error.txt";

inline namespace detail {

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

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

inline double compute_ratio(double time, int V, int E) {
    return 1e9 * time / (E * sqrt(V));
}

} // namespace detail

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
}

void stress_test_general_matching() {
    intd distV(30, 100);
    reald distMp(0.1, 0.5);
    reald distEp(3.0, 15.0);

    LOOP_FOR_DURATION_TRACKED (4s, now) {
        print_time(now, 4s, "stress test general matching");

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
}

void scaling_test_general_matching() {
    vector<vector<stringable>> table;
    table.push_back({"V", "E", "runs", "ratio", "time"});

    auto run = [&](int V, int E) {
        START_ACC(mv);

        LOOP_FOR_DURATION_OR_RUNS_TRACKED (1s, now, 1000, runs) {
            print_time(now, 1s, "scaling test general matching");

            edges_t g = random_exact_undirected_connected(V, E);
            random_relabel_graph_inplace(V, g);

            START(mv);
            micali_vazirani vg(V, g);
            vg.bootstrap();
            vg.max_matching();
            ADD_TIME(mv);
        }

        auto ratio = format("{:.2f}", compute_ratio(EACH_MS(mv, runs), V, E));
        table.push_back({V, E, runs, ratio, EACH_MS(mv, runs)});
    };

    run(200, 300);
    run(200, 2'000);
    run(500, 800);
    run(500, 12'000);
    run(1'000, 2'000);
    run(1'000, 8'000);
    run(1'000, 30'000);
    run(5'000, 7'000);
    run(5'000, 16'000);
    run(5'000, 70'000);
    run(5'000, 230'000);
    run(10'000, 15'000);
    run(10'000, 25'000);
    run(10'000, 150'000);
    run(20'000, 30'000);
    run(20'000, 45'000);
    run(20'000, 400'000);
    run(30'000, 50'000);
    run(30'000, 70'000);
    run(30'000, 550'000);
    run(50'000, 80'000);
    run(50'000, 780'000);
    run(100'000, 150'000);
    run(100'000, 250'000);
    run(100'000, 350'000);
    run(100'000, 1'000'000);
    run(200'000, 300'000);
    run(250'000, 300'000);

    print_time_table(table, "Scaling General matching");
}

void speed_test_general_matching() {
    vector<vector<stringable>> table;
    table.push_back({"V", "E", "runs", "boost", "mv"});

    auto run = [&](int V, int E) {
        START_ACC2(boost, mv);

        LOOP_FOR_DURATION_TRACKED_RUNS (1s, now, runs) {
            print_time(now, 1s, "speed test general matching V,E={},{}", V, E);

            edges_t g = random_exact_undirected_connected(V, E);
            random_relabel_graph_inplace(V, g);
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

        table.push_back({V, E, runs, FORMAT_EACH(boost, runs), FORMAT_EACH(mv, runs)});
    };

    run(50, 70);
    run(100, 150);
    run(200, 300);
    run(500, 800);
    run(500, 3'000);
    run(500, 8'000);
    run(2'000, 5'000);
    run(2'000, 12'000);
    run(5'000, 7'000);
    run(5'000, 30'000);
    run(5'000, 100'000);
    run(5'000, 300'000);
    run(5'000, 1'000'000);
    run(10'000, 15'000);
    run(10'000, 25'000);
    run(20'000, 30'000);
    run(20'000, 45'000);
    run(30'000, 50'000);
    run(30'000, 80'000);
    run(30'000, 300'000);
    run(50'000, 100'000);
    run(100'000, 150'000);

    print_time_table(table, "General matching");
}

int main() {
    RUN_BLOCK(dataset_test_general_matching());
    RUN_BLOCK(stress_test_general_matching());
    RUN_BLOCK(speed_test_general_matching());
    RUN_BLOCK(scaling_test_general_matching());
    return 0;
}
