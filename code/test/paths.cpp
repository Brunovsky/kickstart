#include "test_utils.hpp"
#include "../graphs/shortest_paths.hpp"
#include "../lib/graph_generator.hpp"

void speed_test_positive_shortest_paths() {
    vector<int> Vs = {100, 1000, 5000, 15000};
    vector<int> Es = {2, 3, 5, 10};
    intd weightd(1, 10'000);
    const auto runtime = 20000ms / (Vs.size() * Es.size());
    map<tuple<int, int, string>, string> table;

    auto run = [&](int V, int E) {
        START_ACC3(dijkstra, spfa, bellman);

        LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
            print_time(now, runtime, "stress test paths V,E={},{}", V, V * E);
            auto dg = random_exact_undirected_connected(V, V * E);

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

            START(bellman);
            auto d3 = bellman_ford(0, adj);
            ADD_TIME(bellman);

            assert(d1 == d2 && d1 == d3);
        }

        table[{V, E, "dijkstra"}] = FORMAT_EACH(dijkstra, runs);
        table[{V, E, "spfa"}] = FORMAT_EACH(spfa, runs);
        table[{V, E, "bellman"}] = FORMAT_EACH(bellman, runs);
    };

    for (int V : Vs) {
        for (int E : Es) {
            run(V, E);
        }
    }

    print_time_table(table, "Shortest paths positive");
}

void speed_test_all_positive_shortest_paths() {
    vector<int> Vs = {50, 100, 200, 400, 800};
    vector<int> Es = {2, 3, 5, 8, 12};
    intd weightd(1, 10'000);
    const auto runtime = 20000ms / (Vs.size() * Es.size());
    map<tuple<int, int, string>, string> table;

    auto run = [&](int V, int E) {
        START_ACC3(johnsons, floyd, dijkstra);

        LOOP_FOR_DURATION_TRACKED_RUNS (runtime, now, runs) {
            print_time(now, runtime, "stress test all paths V,E={},{}", V, V * E);
            auto dg = random_exact_undirected_connected(V, V * E);

            vector<vector<pair<int, int>>> adj(V);
            for (auto [u, v] : dg) {
                int w = weightd(mt);
                adj[u].emplace_back(v, w);
                adj[v].emplace_back(u, w);
            }

            START(johnsons);
            auto d1 = johnsons(adj);
            ADD_TIME(johnsons);

            START(floyd);
            auto d2 = floyd_warshall(adj);
            ADD_TIME(floyd);

            START(dijkstra);
            auto d3 = dijkstra_all(adj);
            ADD_TIME(dijkstra);

            assert(d1 == d2);
        }

        table[{V, E, "johnsons"}] = FORMAT_EACH(johnsons, runs);
        table[{V, E, "floyd"}] = FORMAT_EACH(floyd, runs);
        table[{V, E, "dijkstra"}] = FORMAT_EACH(dijkstra, runs);
    };

    for (int V : Vs) {
        for (int E : Es) {
            run(V, E);
        }
    }

    print_time_table(table, "Shortest paths positive");
}

int main() {
    RUN_SHORT(speed_test_positive_shortest_paths());
    RUN_SHORT(speed_test_all_positive_shortest_paths());
    return 0;
}
