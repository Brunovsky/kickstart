#include "test_utils.hpp"
#include "../lib/bipartite.hpp"
#include "../matching/mincost_hungarian.hpp"

inline namespace detail {

template <typename MCBM, typename Costs>
void add_edges(MCBM& mcbm, const edges_t& g, const Costs& costs) {
    int E = g.size();
    for (int i = 0; i < E; i++) {
        mcbm.add(g[i][0], g[i][1], costs[i]);
    }
}

} // namespace detail

inline namespace unit_testing_mincost_matching {

void unit_test_mincost_hungarian() {
    edges_t g;
    vector<int> cost;
    int c;

    g = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    cost = {7, 6, 5, 2};
    mincost_hungarian<int, int> mm1(2, 2);
    add_edges(mm1, g, cost);
    c = mm1.mincost_max_matching();
    assert(c == 9);

    g = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}};
    cost = {5, 4, 6, 3, 2, 1, 4};
    mincost_hungarian<int, int> mm2(3, 3);
    add_edges(mm2, g, cost);
    c = mm2.mincost_max_matching();
    assert(c == 7);

    g = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}};
    cost = {108, 125, 150, 150, 135, 175, 122, 148, 250};
    mincost_hungarian<int, int> mm3(3, 3);
    add_edges(mm3, g, cost);
    c = mm3.mincost_max_matching();
    assert(c == 407);
}

} // namespace unit_testing_mincost_matching

inline namespace speed_testing_mincost_matching {

void speed_test_mincost_matching_run(bipartite_graph_kind i, int S) {
    START_ACC(hungarian);
    int imperfect = 0;
    double avg_mc = 0;

    LOOP_FOR_DURATION_TRACKED_RUNS(1s, now, runs) {
        print_time(now, 1s, 50ms, bipartite_kind_name[i]);

        auto graph = generate_bipartite_graph(i, S);
        add_cost_bipartite_graph(graph, 10'000'000, 300'000'000);

        START(hungarian);
        mincost_hungarian<int, long> g0(graph.U, graph.V);
        add_edges(g0, graph.g, graph.cost);
        g0.pad_reverse();
        auto ans0 = g0.mincost_max_matching();
        ADD_TIME(hungarian);

        auto ans1 = g0.mincost_max_matching(); // again

        mincost_hungarian<long, long> g1(graph.U, graph.V);
        add_edges(g1, graph.g, graph.cost);
        g1.pad_reverse();
        auto ans2 = g1.mincost_max_matching();

        assert(ans0 == ans1 && ans0 == ans2);

        imperfect += ans0 == -1;
        avg_mc += ans0 == -1 ? 0 : ans0 / (graph.U + graph.V);
    }
    avg_mc /= runs;

    double imperfect_percent = 100.0 * imperfect / runs;
    printcl(" {:>8}us -- hungarian -- {:14.1f} avg. -- {:5.1f}% imperfect -- {}\n",
            EACH_US(hungarian, runs), avg_mc, imperfect_percent, bipartite_kind_name[i]);
}

void speed_test_mincost_matching() {
    static const vector<int> sizes = {100, 250, 800, 1500, 3000};

    for (int n = 0; n < int(sizes.size()); n++) {
        print("speed test group S={}, x={}\n", sizes[n]);
        for (int i = 0; i < int(BG_END); i++) {
            speed_test_mincost_matching_run(bipartite_graph_kind(i), sizes[n]);
        }
    }
}

} // namespace speed_testing_mincost_matching

int main() {
    RUN_SHORT(unit_test_mincost_hungarian());
    RUN_BLOCK(speed_test_mincost_matching());
    return 0;
}
