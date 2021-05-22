#include "test_utils.hpp"
#include "../graphs/min_arborescence.hpp"
#include "../lib/graph_generator.hpp"

namespace kactl {

typedef vector<int> vi;

struct Edge {
    int id;
    int a, b;
    int w;
};
struct Node { /// lazy skew heap node
    Edge key;
    Node *l = nullptr, *r = nullptr;
    int delta = 0;
    void prop() {
        key.w += delta;
        if (l)
            l->delta += delta;
        if (r)
            r->delta += delta;
        delta = 0;
    }
    Edge top() {
        prop();
        return key;
    }
};
Node* merge(Node* a, Node* b) {
    if (!a || !b)
        return a ? a : b;
    a->prop(), b->prop();
    if (a->key.w > b->key.w)
        swap(a, b);
    swap(a->l, (a->r = merge(b, a->r)));
    return a;
}
void pop(Node*& a) {
    a->prop();
    a = merge(a->l, a->r);
}

pair<int, vector<int>> dmst(int n, int r, vector<Edge>& g) {
    disjoint_set_rollback dsu(n);
    vector<Node*> heap(n);
    for (Edge e : g)
        heap[e.b] = merge(heap[e.b], new Node{e});
    int res = 0;
    vi seen(n, -1), path(n), par(n);
    seen[r] = r;
    vector<Edge> Q(n), in(n, {-1, -1, -1, 0});
    deque<tuple<int, int, vector<Edge>>> cycs;
    for (int s = 0; s < n; s++) {
        int u = s, qi = 0, w;
        while (seen[u] < 0) {
            if (!heap[u]) {
                return {-1, {}};
            }
            Edge e = heap[u]->top();
            heap[u]->delta -= e.w, pop(heap[u]);
            Q[qi] = e, path[qi++] = u, seen[u] = s;
            res += e.w, u = dsu.find(e.a);
            if (seen[u] == s) { /// found cycle, contract
                Node* cyc = nullptr;
                int end = qi, time = dsu.time();
                do
                    cyc = merge(cyc, heap[w = path[--qi]]);
                while (dsu.join(u, w));
                u = dsu.find(u), heap[u] = cyc, seen[u] = -1;
                cycs.push_front({u, time, {&Q[qi], &Q[end]}});
            }
        }
        for (int i = 0; i < qi; i++)
            in[dsu.find(Q[i].b)] = Q[i];
    }

    for (auto& [u, t, comp] : cycs) { // restore sol (optional)
        dsu.rollback(t);
        Edge inEdge = in[u];
        for (auto& e : comp)
            in[dsu.find(e.b)] = e;
        in[dsu.find(inEdge.b)] = inEdge;
    }
    for (int i = 0; i < n; i++)
        par[i] = in[i].id;
    return {res, par};
}

} // namespace kactl

inline namespace detail {

template <typename Cost>
auto convert_to_kactl(const edges_t& G, const vector<Cost>& cost) {
    int E = G.size();
    vector<kactl::Edge> edges(E);
    for (int e = 0; e < E; e++) {
        edges[e] = {e, G[e][0], G[e][1], cost[e]};
    }
    return edges;
}

template <typename Cost, typename CostSum = Cost>
auto get_cost(int V, const vector<int>& in, const vector<Cost>& cost) {
    Cost total = 0;
    assert(V == int(in.size()));
    for (int i = 0; i < V; i++) {
        if (in[i] >= 0) {
            total += cost[in[i]];
        }
    }
    return total;
}

bool verify_inedges(int V, const vector<int>& in, const edges_t& g) {
    int errors = 0, non = 0;
    assert(V == int(in.size()));
    for (int i = 0; i < V; i++) {
        if (in[i] >= 0) {
            errors += g[in[i]][1] != i;
        } else {
            non++;
        }
    }
    return errors == 0 && non == 1;
}

} // namespace detail

inline namespace stress_testing_min_arborescence {

void stress_test_min_arborescence(double F = 10.0, int T = 500) {
    int minV = 100 * F, maxV = 150 * F;
    int minE = 600 * F, maxE = 800 * F;
    int mincost = 1, maxcost = 1000;
    intd distV(minV, maxV);
    intd distE(minE, maxE);
    int avgV = (minV + maxV) / 2, avgE = (minE + maxE) / 2;

    START_ACC(kactl);
    START_ACC(min_arbo);
    START_ACC(min_arbo_extract);
    long sum_costs = 0;

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test min arborescence");

        int V = distV(mt), E = distE(mt);
        auto G = random_exact_directed_connected(V, E);
        auto cost = int_gen<int>(E, mincost, maxcost);
        auto K = convert_to_kactl(G, cost);
        int root = intd(0, V - 1)(mt);

        START(kactl);
        auto [ans1, in1] = kactl::dmst(V, root, K);
        ADD_TIME(kactl);

        START(min_arbo_extract);
        auto [ans3, in3] = *min_arborescence<int>(V, root, G, cost);
        ADD_TIME(min_arbo_extract);

        START(min_arbo);
        auto ans2 = *min_arborescence_cost<int>(V, root, G, cost);
        ADD_TIME(min_arbo);

        assert(verify_inedges(V, in1, G));
        assert(verify_inedges(V, in3, G));
        assert(ans1 == get_cost(V, in1, cost));
        assert(ans3 == get_cost(V, in3, cost));
        assert(ans1 == ans2 && ans1 == ans3);

        sum_costs += ans1;
    }

    double avg_cost = 1.0 * sum_costs / T;
    double exp_cost = (1.0 * (maxcost - mincost) * avgV / avgE + mincost) * (avgV - 1);

    PRINT_TIME(kactl);
    PRINT_TIME(min_arbo);
    PRINT_TIME(min_arbo_extract);
    print(" {:8.1f} average cost\n", avg_cost);
    print(" {:8.1f} expected cost\n", exp_cost);
}

} // namespace stress_testing_min_arborescence

inline namespace unit_testing_min_arborescence {

void unit_test_min_arborescence() {
    int V = 20, E = 40, root = 0;
    auto G = random_exact_directed_connected(V, E);
    auto cost = int_gen<int>(E, 5, 80);

    auto input = convert_to_kactl(G, cost);

    auto [ans1, in1] = kactl::dmst(V, root, input);
    print("ans 1: {}\n", ans1);
    print("in1: {}\n", in1);
    auto cost1 = get_cost(V, in1, cost);
    assert(cost1 == ans1);

    auto ans2 = *min_arborescence_cost(V, root, G, cost);
    print("ans 2: {}\n", ans2);

    auto [ans3, in3] = *min_arborescence(V, root, G, cost);
    print("ans 3: {}\n", ans3);
    print("in3: {}\n", in3);
    auto cost3 = get_cost(V, in3, cost);
    assert(cost3 == ans3);

    assert(ans1 == ans2 && ans1 == ans3);
}

} // namespace unit_testing_min_arborescence

int main() {
    RUN_SHORT(unit_test_min_arborescence());
    RUN_SHORT(stress_test_min_arborescence());
    return 0;
}
