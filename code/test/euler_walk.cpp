#include "test_utils.hpp"
#include "../graphs/euler_tour.hpp"
#include "../lib/graph_generator.hpp"

void unit_test_directed_euler_tour(int V = 8, int k = 4) {
    auto g = random_regular_directed_connected(V, k);
    int E = g.size();
    assert(E == V * k);

    auto path = build_directed_euler_tour(0, g);
    assert(int(path.size()) == E);

    vector<bool> seen(E, false);

    for (int i = 0; i < E; i++) {
        int e = path[i];
        if (i > 0) {
            int f = path[i - 1];
            assert(g[f][1] == g[e][0]); // edges link
        }
        assert(!seen[e]);
        seen[e] = true;
    }
}

void unit_test_undirected_euler_tour(int V = 8, int k = 4) {
    auto g = random_regular_connected(V, k);
    int E = g.size();
    assert(E == V * k / 2);

    auto path = build_undirected_euler_tour(0, g);
    assert(int(path.size()) == E);

    vector<bool> seen(E, false);

    for (int i = 0; i < E; i++) {
        int e = path[i];
        auto [v0, v1] = g[e];
        if (i > 0) {
            int f = path[i - 1];
            auto [u0, u1] = g[f];
            assert(u0 == v0 || u0 == v1 || u1 == v0 || u1 == v1);
            // ...u0 -> u1==v0 -> v1...
        }
        assert(!seen[e]);
        seen[e] = true;
    }
}

int main() {
    RUN_SHORT(unit_test_directed_euler_tour(8, 4));
    RUN_SHORT(unit_test_directed_euler_tour(20, 6));
    RUN_SHORT(unit_test_directed_euler_tour(200, 20));
    RUN_SHORT(unit_test_undirected_euler_tour(8, 4));
    RUN_SHORT(unit_test_undirected_euler_tour(20, 6));
    RUN_SHORT(unit_test_undirected_euler_tour(200, 20));
    return 0;
}
