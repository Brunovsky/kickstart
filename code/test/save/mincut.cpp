#include "../mincut.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"

// *****

auto graph_unit_1() {
    cost_graph g(8);
    g.add(1, 2, 2);
    g.add(1, 5, 3);
    g.add(2, 3, 3);
    g.add(2, 5, 2);
    g.add(2, 6, 2);
    g.add(3, 4, 4);
    g.add(3, 7, 2);
    g.add(4, 7, 2);
    g.add(4, 0, 2);
    g.add(5, 6, 3);
    g.add(6, 7, 1);
    g.add(7, 0, 3);
    return g;
}

template <typename MC>
MC convert(const cost_graph& g) {
    MC mc(g.V);
    for (int e = 0; e < g.E; e++) {
        int u = g.source[e], v = g.target[e], c = g.cost[e];
        mc.add(u, v, c);
    }
    return mc;
}

void test_unit() {
    stoer_wagner sw = convert<stoer_wagner>(graph_unit_1());
    auto sol = sw.minimum_cut(3);
    print("stoer_wagner 1 mincut: {}, {}\n", sol.first, sol.second);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_unit();
    return 0;
}
