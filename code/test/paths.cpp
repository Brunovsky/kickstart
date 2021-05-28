#include "test_utils.hpp"
#include "../graphs/shortest_paths.hpp"
#include "../lib/graph_generator.hpp"

inline namespace stress_testing_positive_paths {

void stress_test_positive_shortest_paths() {
    intd weightd(1, 10'000);
    intd distV(6000, 8000);
    intd distE(30000, 45000);

    START_ACC(dijkstra);
    START_ACC(spfa);
    START_ACC(bellman_ford);

    LOOP_FOR_DURATION_TRACKED_RUNS(5s, now, runs) {
        print_time(now, 5s, 50ms, "stress test positive shortest paths");
        int V = distV(mt), E = distE(mt);
        auto dg = random_exact_undirected_connected(V, E);

        vector<vector<pair<int, int>>> adj(V);
        for (auto [u, v] : dg) {
            int w = weightd(mt);
            adj[u].emplace_back(v, w);
            adj[v].emplace_back(u, w);
        }

        START(dijkstra);
        auto d1 = dijkstra(0, adj);
        ADD_TIME(dijkstra);

        START(spfa);
        auto d2 = spfa(0, adj);
        ADD_TIME(spfa);

        START(bellman_ford);
        auto d3 = bellman_ford(0, adj);
        ADD_TIME(bellman_ford);

        assert(d1 == d2 && d1 == d3);
    }

    PRINT_EACH_US(dijkstra, runs);
    PRINT_EACH_US(spfa, runs);
    PRINT_EACH_US(bellman_ford, runs);
}

void stress_test_all_positive_shortest_paths() {
    intd weightd(1, 10'000);
    intd distV(700, 800);
    intd distE(3000, 4500);

    START_ACC(johnsons);
    START_ACC(floyd_warshall);
    START_ACC(dijkstra_all);

    LOOP_FOR_DURATION_TRACKED_RUNS(15s, now, runs) {
        print_time(now, 15s, 50ms, "stress test all positive shortest paths");
        int V = distV(mt), E = distE(mt);
        auto dg = random_exact_undirected_connected(V, E);

        vector<vector<pair<int, int>>> adj(V);
        for (auto [u, v] : dg) {
            int w = weightd(mt);
            adj[u].emplace_back(v, w);
            adj[v].emplace_back(u, w);
        }

        START(johnsons);
        auto d1 = johnsons(adj);
        ADD_TIME(johnsons);

        START(floyd_warshall);
        auto d2 = floyd_warshall(adj);
        ADD_TIME(floyd_warshall);

        START(dijkstra_all);
        auto d3 = dijkstra_all(adj);
        ADD_TIME(dijkstra_all);

        assert(d1 == d2);
    }

    PRINT_EACH_MS(johnsons, runs);
    PRINT_EACH_MS(floyd_warshall, runs);
    PRINT_EACH_MS(dijkstra_all, runs);
}

} // namespace stress_testing_positive_paths

int main() {
    RUN_SHORT(stress_test_positive_shortest_paths());
    RUN_SHORT(stress_test_all_positive_shortest_paths());
    return 0;
}
