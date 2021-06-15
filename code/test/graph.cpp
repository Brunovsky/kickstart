#include "test_utils.hpp"
#include "../lib/bipartite.hpp"
#include "../lib/circulation.hpp"
#include "../lib/flow.hpp"

void balance_test_generate_flow() {
    // ¹²³⁴⁵⁶⁷⁸⁹⁰ √
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Flow Networks");
        //                      dinitz   mpm   relabel edmonds
        print("{1:<{0}} {2:5} --  V²E     V³     V²√E  VE²lnV  --\n", 58, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(FN_END); i++) {
            long V_sum = 0, E_sum = 0;

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (500ms, now, 50, runs) {
                auto fn = generate_flow_network(flow_network_kind(i), S);
                V_sum += fn.V, E_sum += fn.E;
            }
            V_sum /= runs, E_sum /= runs;

            double V = log2(V_sum + 1), E = log2(E_sum + 1), logV = log2(V + 1);
            double W = 9 * V + 4 * E + 2 * logV;
            sum_W += W;

            auto& name = flow_kind_name[i];
            print("{:4} {:36} -- V:{:<6} E:{:<8} --", i, name, V_sum, E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * V + E, 3 * V, 2 * V + .5 * E, V + 2 * E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 96, "", sum_W / int(FN_END));
    }
}

void balance_test_generate_circulation() {
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Circulation Networks");
        //                      dinitz   mpm   relabel mincost relabel
        print("{1:<{0}} {2:5} --  V²E     V³     V²√E  V²ElnV  --\n", 58, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(CN_END); i++) {
            long V_sum = 0, E_sum = 0;

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (500ms, now, 50, runs) {
                auto fn = generate_circulation_network(circulation_network_kind(i), S);
                V_sum += fn.V, E_sum += fn.E;
            }
            V_sum /= runs, E_sum /= runs;

            double V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 9.5 * V + 3 * E + 2 * logV;
            sum_W += W;

            auto& name = circulation_kind_name[i];
            print("{:4} {:36} -- V:{:<6} E:{:<8} --", i, name, V_sum, E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * V + E, 3 * V, 2 * V + .5 * E, 2 * V + E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 96, "", sum_W / int(CN_END));
    }
}

void balance_test_generate_bipartite() {
    for (int S : {100, 200, 500, 1000, 2000, 5000, 15000, 30000}) {
        auto header = format("Bipartite Graphs");
        print("{1:<{0}} {2:5} --  V²E    V³     V²√E  V²ElnV  --\n", 68, header, S);

        double sum_W = 0;

        for (int i = 0; i < int(BG_END); i++) {
            long U_sum = 0, V_sum = 0, E_sum = 0;

            LOOP_FOR_DURATION_OR_RUNS_TRACKED (500ms, now, 50, runs) {
                auto fn = generate_bipartite_graph(bipartite_graph_kind(i), S);
                U_sum += fn.U, V_sum += fn.V, E_sum += fn.E;
            }
            V_sum /= runs, E_sum /= runs;

            double U = log2(U_sum), V = log2(V_sum), E = log2(E_sum), logV = log2(V);
            double W = 9.5 * (U + V) + 3 * E + 2 * logV;
            sum_W += W;

            auto& name = bipartite_kind_name[i];
            print("{:4} {:36} -- U:{:<6} V:{:<6} E:{:<8} --", i, name, U_sum, V_sum,
                  E_sum);
            print(" {:4.1f}    {:4.1f}    {:4.1f}    {:4.1f}  -- {:5.1f}\n", //
                  2 * U + E, 3 * V, 2 * V + .5 * E, 2 * V + E + logV, W);
        }
        print("{1:<{0}} --- {2:5.1f}\n", 105, "", sum_W / int(BG_END));
    }
}

int main() {
    RUN_BLOCK(balance_test_generate_flow());
    RUN_BLOCK(balance_test_generate_circulation());
    RUN_BLOCK(balance_test_generate_bipartite());
    return 0;
}
