#include "test_utils.hpp"
#include "../lib/bipartite.hpp"
#include "../matching/mincost_hungarian.hpp"

inline namespace unit_testing_mincost_matching {

void unit_test_mincost_matching() {
    edges_t g;
    vector<long> cost;
    long c;

    g = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    cost = {7, 6, 5, 2};
    mincost_hungarian mm(2, 2, g, cost);
    c = mm.mincost_max_matching();
    assert(c == 9);

    g = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}};
    cost = {5, 4, 6, 3, 2, 1, 4};
    mm = mincost_hungarian(3, 3, g, cost);
    c = mm.mincost_max_matching();
    assert(c == 7);

    g = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}};
    cost = {108, 125, 150, 150, 135, 175, 122, 148, 250};
    mm = mincost_hungarian(3, 3, g, cost);
    c = mm.mincost_max_matching();
    assert(c == 407);
}

} // namespace unit_testing_mincost_matching

inline namespace speed_testing_mincost_matching {

void speed_test_mincost_matching_run(bipartite_graph_kind i, int S, int T) {
    START_ACC(hungarian);
    int imperfect = 0;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, bipartite_kind_name[i]);
        auto graph = generate_bipartite_graph(i, S);
        add_cost_bipartite_graph(graph, 100'000'000'000);

        START(hungarian);
        mincost_hungarian mcm(graph.U, graph.V, graph.g, graph.cost);
        auto mc = mcm.mincost_max_matching();
        ADD_TIME(hungarian);

        imperfect += mc == -1;
    }

    clear_line();
    print(" {:>8}ms -- hungarian -- ({:5.1f}% imperfect) {}\n", TIME_MS(hungarian),
          100.0 * imperfect / T, bipartite_kind_name[i]);
}

void speed_test_mincost_matching() {
    static constexpr int N = 5;
    static constexpr int sizes[] = {100, 250, 800, 1500, 3000};
    static constexpr int amounts[] = {300, 100, 20, 10, 4};
    for (int n = 0; n < N; n++) {
        print("speed test group S={}, x={}\n", sizes[n], amounts[n]);
        for (int i = 0; i < int(BG_END); i++) {
            speed_test_mincost_matching_run(bipartite_graph_kind(i), sizes[n],
                                            amounts[n]);
        }
    }
}

} // namespace speed_testing_mincost_matching

int main() {
    RUN_SHORT(unit_test_mincost_matching());
    RUN_BLOCK(speed_test_mincost_matching());
    return 0;
}
