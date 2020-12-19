#include <fmt/format.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/isomorphism.hpp>

#include "../graph_generator.hpp"
#include "../isomorphism.hpp"

using fmt::print;
using bgraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;

// *****

bgraph to_boost(const graph& g) {
    bgraph bg(g.V);
    for (int u = 0; u < g.V; u++) {
        for (int v : g.adj[u]) {
            add_edge(u, v, bg);
        }
    }
    return bg;
}

bool boost_test(graph& g1, graph& g2) {
    auto bg1 = to_boost(g1);
    auto bg2 = to_boost(g2);
    return boost::isomorphism(bg1, bg2);
}

void test_true() {
    int R = 10000;
    int V = 15;
    reald distp(0.1, 0.9);

    for (int i = 1; i <= R; i++) {
        print("\rTrue test {}...", i);
        auto g1 = random_uniform_undirected_connected(V, distp(mt));
        auto g2 = relabel(g1);
        assert(isomorphic(g1, g2));
    }
}

void show(const string& name, int fp, int positives) {
    double rate = positives ? 100.0 * fp / positives : -1;
    print("\r--- {}  {:4.1f}%  f={:<5} p={:<5}", name, rate, fp, positives);
}

template <typename Gn>
void run_test(const string& name, int R, Gn&& gn) {
    int fp = 0, positives = 0;

    for (int i = 1; i <= R; i++) {
        auto [g1, g2] = gn();

        if (isomorphic(g1, g2)) {
            positives++, fp += !boost_test(g1, g2);
            show(name, fp, positives);
        }
    }

    show(name, fp, positives);
    print("\n");
}

void test_false() {
    run_test("regular sparse 12V", 40000, []() {
        int V = 12;
        intd distk(3, 5);
        int k = distk(mt);
        auto g1 = random_regular(V, k);
        auto g2 = random_regular(V, k);
        return pair<graph, graph>{g1, g2};
    });
    run_test("regular moderate 12V", 20000, []() {
        int V = 12;
        intd distk(6, 8);
        int k = distk(mt);
        auto g1 = random_regular(V, k);
        auto g2 = random_regular(V, k);
        return pair<graph, graph>{g1, g2};
    });
}

int main() {
    setbuf(stdout, nullptr);
    test_true();
    test_false();
    return 0;
}
