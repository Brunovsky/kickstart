#include "../struct/integer_heaps.hpp"

#include "../formatting.hpp"
#include "../lib/graph_generator.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

template <typename Compare>
ostream& operator<<(ostream& out, pairing_int_heap<Compare> heap) {
    vector<int> nums;
    while (!heap.empty())
        nums.push_back(heap.pop());
    return out << nums;
}

template <typename Compare>
ostream& operator<<(ostream& out, binary_int_heap<Compare> heap) {
    vector<int> nums;
    while (!heap.empty())
        nums.push_back(heap.pop());
    return out << nums;
}

template <template <typename> typename Heap, bool adjust = true>
void stress_test_int_heap(int N = 60, int T = 300'000) {
    const int K = 100'000;
    assert(2LL * K * N <= INT_MAX);
    int super = N * K + 1, bound = N * K;
    intd numd(0, N - 1), vald(0, K);

    vector<int> weight(N);
    Heap heap(N, greater_container(weight));
    set<int, greater_container<vector<int>>> nums(weight);
    long size_sum = 0;

    auto getnext = [&](int n) {
        if constexpr (adjust) {
            return (void)n, vald(mt) * N + n;
        } else if (weight[n] >= bound) {
            return super++;
        } else {
            return weight[n] = intd(weight[n] / N, K)(mt) * N + n;
        }
    };

    for (int n = 0; n < N; n++) {
        weight[n] = getnext(n);
        heap.push(n), nums.insert(n);
    }
    for (int n = 0; n < N; n++) {
        assert(heap.contains(n));
    }

    reald actiond(0, 1);
    double action;

#define STRESS_VERIFY(ok) verify(ok, #ok, __LINE__)

    auto verify = [&](bool ok, string text, int line) {
        if (!ok) {
            print("{}: Assertion '{}' failed.\nHeap: {}\nNums: {}\nAction: {}\n", line,
                  text, heap, nums, action);
            abort();
        }
    };

    for (int t = 0; t < T; t++) {
        action = actiond(mt);
        if (t % 500 == 0) {
            print_progress(t, T, "stress heap");
        }

        if (action < 0.0000) { // clear
            nums.clear();
            heap.clear();
        } else if (action < 0.40) { // push
            int n = numd(mt);
            if (!nums.count(n)) {
                STRESS_VERIFY(!heap.contains(n));
                weight[n] = getnext(n);
                heap.push(n);
                nums.insert(n);
            }
        } else if (action < 0.80) { // improve or adjust
            int n = numd(mt);
            if (nums.count(n)) {
                STRESS_VERIFY(heap.contains(n));
                nums.erase(n);
                weight[n] = getnext(n);
                if constexpr (adjust) {
                    heap.adjust(n);
                } else {
                    heap.improve(n);
                }
                nums.insert(n);
            }
        } else if (action < 1.0) { // pop
            if (!nums.empty()) {
                STRESS_VERIFY(!heap.empty());
                int n = *nums.begin();
                nums.erase(nums.begin());
                int m = heap.pop();
                STRESS_VERIFY(n == m);
            }
        }

        size_sum += nums.size();

        STRESS_VERIFY(heap.empty() == nums.empty());
        STRESS_VERIFY(heap.empty() || heap.top() == *nums.begin());
    }

    double avg = 1.0 * size_sum / T;
    clear_line();
    print("average size: {:.2f} ({:.2f}%)\n", avg, 100.0 * avg / N);
}

void unit_test_pairing_heaps() {
    constexpr int R = 5, N = 15;

    long cost[N] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28};
    pairing_int_heaps<greater_container<long*>> heaps(R, N, cost);
    int u;

    heaps.push(0, 1);
    heaps.push(0, 3);

    heaps.push(1, 4);
    heaps.push(1, 5);
    heaps.push(1, 2);
    heaps.push(1, 0);
    u = heaps.pop(1);
    assert(u == 5);

    heaps.merge(0, 1);
    assert(heaps.empty(1));
    u = heaps.pop(0);
    assert(u == 4);
    u = heaps.pop(0);
    assert(u == 3); // 0,1,2 left

    heaps.push(1, 5);
    heaps.push(1, 9);
    u = heaps.pop(1);
    assert(u == 9);

    heaps.push(2, 7);
    heaps.push(2, 8);
    heaps.push(2, 9);
    u = heaps.pop(2);
    assert(u == 9);
    u = heaps.pop(2);
    assert(u == 8);
    assert(heaps.top(2) == 7);

    cost[0] = 5;
    assert(heaps.top(0) == 2);
    heaps.improve(0, 0);
    assert(heaps.top(0) == 0);
    u = heaps.pop(0);
    assert(u == 0);
    u = heaps.pop(0);
    assert(u == 2);

    heaps.push_or_improve(0, 3);
    assert(heaps.top(0) == 3);
}

struct cost_graph {
    int V, E;
    vector<vector<int>> adj;
    edges_t edge;
    vector<long> cost;
};

void run_normal(const cost_graph& g, int s, vector<long>& dist) {
    static constexpr long inf = LONG_MAX / 2;
    int V = g.V;
    dist.assign(V, inf);
    dist[s] = 0;

    using int2 = pair<long, int>;
    priority_queue<int2, vector<int2>, greater<int2>> Q;
    Q.push({0, s});

    while (!Q.empty()) {
        auto [ucost, u] = Q.top();
        Q.pop();
        if (dist[u] < ucost) {
            continue;
        }
        for (auto e : g.adj[u]) {
            int v = g.edge[e][1];
            long cost = dist[u] + g.cost[e];
            if (cost < dist[v]) {
                dist[v] = cost;
                Q.push({cost, v});
            }
        }
    }
}

template <typename Heap>
void run_dijkstra(const cost_graph& g, int s, vector<long>& dist, Heap& Q) {
    static constexpr long inf = LONG_MAX / 2;
    int V = g.V;
    dist.assign(V, inf);
    dist[s] = 0;

    vector<bool> vis(V, false);
    assert(Q.empty());
    Q.push(s);

    while (!Q.empty()) {
        int u = Q.pop();
        for (auto e : g.adj[u]) {
            int v = g.edge[e][1];
            long cost = dist[u] + g.cost[e];
            if (cost < dist[v]) {
                dist[v] = cost;
                Q.push_or_improve(v);
            }
        }
    }
}

void speed_test_int_heaps(int T = 10000) {
    intd distV(50, 60);
    reald density(8.0, 12.0);
    longd costd(1, 100'000);
    int step = 10;

    START_ACC(dijkstra);
    START_ACC(pairing);
    START_ACC(binary);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "speed test int heaps");

        cost_graph g;
        int V = g.V = distV(mt);
        double p = density(mt) / V;
        g.edge = random_uniform_rooted_dag_connected(V, p);
        g.E = g.edge.size(), g.adj = make_adjacency_lists_directed(g.edge, V);
        g.cost.resize(g.E);
        for (int e = 0; e < g.E; e++)
            g.cost[e] = costd(mt);

        vector<long> dist[3];
        pairing_int_heap<less_container<vector<long>>> pairing_heap(V, dist[1]);
        binary_int_heap<less_container<vector<long>>> binary_heap(V, dist[2]);

        START(dijkstra);
        for (int s = 0; s < V; s += step)
            run_normal(g, s, dist[0]);
        ADD_TIME(dijkstra);

        START(pairing);
        for (int s = 0; s < V; s += step)
            run_dijkstra(g, s, dist[1], pairing_heap);
        ADD_TIME(pairing);

        START(binary);
        for (int s = 0; s < V; s += step)
            run_dijkstra(g, s, dist[2], binary_heap);
        ADD_TIME(binary);

        if (dist[0] != dist[1] || dist[0] != dist[2]) {
            fail("wrong dist vectors\n0: {}\n1: {}\n2: {}\n", dist[0], dist[1], dist[2]);
        }
    }

    PRINT_TIME(dijkstra);
    PRINT_TIME(pairing);
    PRINT_TIME(binary);
}

int main() {
    RUN_BLOCK((stress_test_int_heap<binary_int_heap, false>()));
    RUN_BLOCK((stress_test_int_heap<pairing_int_heap, false>()));
    RUN_BLOCK((stress_test_int_heap<pairing_int_heap, true>()));
    RUN_SHORT(unit_test_pairing_heaps());
    RUN_BLOCK(speed_test_int_heaps());
    return 0;
}
