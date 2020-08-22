#include "../general_matching.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "../graph_generator.hpp"

using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using matemap_t = std::vector<boost::graph_traits<bgraph>::vertex_descriptor>;

// *****

int N, R = 300;
vector<string> names;
vector<graph> graphs;
vector<int> actual;

void read(string filename = "datasets/general_matching.txt") {
    ifstream file(filename);
    assert(file.is_open());
    file >> N >> ws;

    names.resize(N);
    actual.resize(N);
    graphs.clear();

    for (int i = 0; i < N; i++) {
        int V, E;
        getline(file, names[i]);
        file >> V >> E >> actual[i] >> ws;

        graph g(V);
        for (int e = 0; e < E; e++) {
            int u, v;
            file >> u >> v >> ws;
            g.add(u, v);
        }
        graphs.push_back(move(g));
    }
}

micali_vazirani to_vazirani(const graph& g) {
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

/**
 * Generate sparse graphs with V1..V2 vertices and collect those that have at least k
 * unmatched vertices in the maximum matching
 */
void generate_hard_cases(int G = 1000, int k = 5, int V1 = 40, int V2 = 70) {
    printf("Generating %d cases...", G);
    ofstream file("datasets/general_matching_hard.txt");
    ofstream dot("datasets/general_matching_hard.dot");
    assert(file.is_open() && dot.is_open());
    file << G << "\n\n";
    dot << G << "\n\n";

    intd distv(V1, V2);
    int total_cnt = 0;

    for (int i = 0; i < G; i++) {
        bgraph bg;
        graph g;
        int V, E, maximum;
        int cur_cnt = 0;
        do {
            V = distv(mt);
            intd diste(3 * V / 2, 6 * V);
            E = diste(mt);
            g = generate_exact_undirected(V, E);
            bg = to_boost(g);
            maximum = boost_matching_size(bg);
            printf("\rGenerated %5d %5d...\r", cur_cnt++, total_cnt++);
        } while (maximum + k > V);
        printf("Generated %2dth graph with V=%2d and E=%2d...\n", i + 1, V, E);

        file << "# Generated, tested by boost #"s + to_string(i + 1) << "\n";
        file << to_simple_compact(g, to_string(maximum)) << endl;
        dot << "# Generated, tested by boost #"s + to_string(i + 1) << "\n";
        dot << to_dot_compact(g, false) << endl;
    }

    printf("Done in %d iterations\n", total_cnt);
}

void test() {
    printf("===== GENERAL MATCHING DATASET TESTS =====\n answer | actual\n");

    for (int i = 0; i < N; i++) {
        auto& g = graphs[i];
        auto name = names[i].data();
        auto vg = to_vazirani(g);
        int card = vg.max_matching();
        printf(" %6d | %6d   %4d%5d  %s\n", card, actual[i], g.V, g.E, name);
    }
}

void random_test() {
    printf("===== GENERAL MATCHING BOOST TESTS =====\n  n  | answer | boost\n");

    intd distv(1, 30);
    reald sparse(4.0, 9.0);

    for (int i = 0; i < R; i++) {
        int V = distv(mt);
        auto g = generate_uniform_undirected(V, sparse(mt) / V);
        bgraph bg = to_boost(g);
        micali_vazirani vg = to_vazirani(g);
        int bcard = boost_matching_size(bg);
        int vcard = vg.max_matching();
        printf("%4d | %6d | %6d   %4d %6d\n", i + 1, vcard, bcard, g.V, g.E);
    }
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    // read();
    // test();
    read("datasets/general_matching_hard.txt");
    test();
    // generate_hard_cases();
    // random_test();
    // test_immediate_blossom();
    return 0;
}
