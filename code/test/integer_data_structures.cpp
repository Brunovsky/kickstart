#include "../integer_data_structures.hpp"

#include "../debug_print.hpp"
#include "../graph_convert.hpp"
#include "../graph_generator.hpp"
#include "chrono.hpp"

// *****

using int2 = array<int, 2>;

void test_dancing_links() {
    dancing_links_matrix dl(6, 6);

    assert(dl.north({4, 4}) == int2({3, 4}));
    assert(dl.south({4, 4}) == int2({5, 4}));
    assert(dl.west({4, 4}) == int2({4, 3}));
    assert(dl.west({5, 1}) == int2({5, 0}));
    assert(dl.east({4, 4}) == int2({4, 5}));
    assert(dl.east({2, 5}) == int2({2, 6}));
    dl.remove({3, 4});
    assert(dl.north({4, 4}) == int2({2, 4}));

    print("OK dancing_links\n");
}

void test_forward_lists() {
    forward_lists fl(6, 20);
    //                 0  1  2  3  4  5
    fl.push(2, 3); //  3     3
    fl.push(2, 4); //  4     4
    fl.push(2, 7); //  7     7
    fl.push(2, 9); //        9
    fl.push(3, 5); //           5     5
    fl.push(4, 8); //              8
    fl.push(3, 1); //           1     1
    fl.push(2, 0); //        0
    fl.push(1, 2); //     2
    fl.push(3, 6); //           6

    vector<vector<int>> seen(6);
    for (int l = 0; l < 6; l++)
        FOR_EACH_IN_FORWARD_LIST (i, l, fl)
            seen[l].push_back(i);

    // print("forward_lists\n"), debugh(seen);
    assert(seen[0] == vector<int>({}));
    assert(seen[1] == vector<int>({2}));
    assert(seen[2] == vector<int>({0, 9, 7, 4, 3}));
    assert(seen[3] == vector<int>({6, 1, 5}));
    assert(seen[4] == vector<int>({8}));
    assert(seen[5] == vector<int>({}));

    print("OK forward_list\n");
}

void test_linked_lists() {
    linked_lists ll(5, 20);
    ll.push_back(0, 1);
    ll.push_back(0, 2);
    ll.push_front(0, 3); // 0: (3->1->2)

    ll.push_front(1, 4);
    ll.push_back(3, 5);
    ll.splice_back(1, 3);
    ll.push_back(3, 6);
    ll.splice_after(5, 3);
    ll.push_front(1, 7); // 1: (7->4->5->6)

    ll.splice_after(1, 1); // 0: (3->1->7->4->5->6->2),  1: ()

    ll.push_front(2, 8);
    ll.push_front(2, 9);
    ll.push_back(2, 10);     // 2: (9->8->10)
    ll.insert_before(8, 11); // 2: (9->11->8->10)
    ll.insert_after(8, 12);  // 2: (9->11->8->12->10)

    ll.push_back(3, 13);
    ll.push_back(3, 14);
    ll.push_back(3, 15); // 3: (13->14->15)

    // 4 is empty here
    ll.splice_before(14, 4);
    ll.splice_after(14, 4);
    ll.splice_before(13, 4);
    ll.splice_after(15, 4);
    ll.splice_front(3, 4);
    ll.splice_back(3, 4); // 3: (13->14->15)

    ll.splice_before(9, 3); // 2: (13->14->15->9->11->8->12->10)
    ll.push_front(3, 16);
    ll.push_front(3, 17);
    ll.splice_back(4, 3); // move 4 to 3
    ll.push_front(4, 18); // 4: (18->17->16)
    ll.splice_back(2, 4); // 2: (13->14->15->9->11->8->12->10->18->17->16)
    ll.push_back(4, 19);
    ll.splice_before(10, 4); // 2: (13->14->15->9->11->8->12->19->10->18->17->16)

    vector<vector<int>> fw(5), bw(5);
    for (int l = 0; l < 5; l++) {
        FOR_EACH_IN_LINKED_LIST (i, l, ll)
            fw[l].push_back(i);
        FOR_EACH_IN_LINKED_LIST_REVERSE (i, l, ll)
            bw[l].push_back(i);
    }

    // print("linked_lists\n"), debugh(fw), debugh(bw);
    assert(fw[0] == vector<int>({3, 1, 7, 4, 5, 6, 2}));
    assert(fw[1] == vector<int>({}));
    assert(fw[2] == vector<int>({13, 14, 15, 9, 11, 8, 12, 19, 10, 18, 17, 16}));
    assert(fw[3] == vector<int>({}));
    assert(fw[4] == vector<int>({}));
    assert(bw[0] == vector<int>({2, 6, 5, 4, 7, 1, 3}));
    assert(bw[1] == vector<int>({}));
    assert(bw[2] == vector<int>({16, 17, 18, 10, 19, 12, 8, 11, 9, 15, 14, 13}));
    assert(bw[3] == vector<int>({}));
    assert(bw[4] == vector<int>({}));

    print("OK linked_list\n");
}

void test_freelist() {
    linked_lists ll(6, 20);
    int_freelist fr(20);

    int a = fr.request(); // 0
    assert(a == 0);
    ll.push_back(a, 1);
    ll.push_back(a, 2);
    ll.push_front(a, 3); // 0: (3->1->2)

    int b = fr.request(); // 1
    int c = fr.request(); // 2
    assert(b == 1 && c == 2);
    ll.push_front(b, 4);
    ll.push_back(c, 5);
    ll.splice_back(b, c);
    ll.push_back(c, 6);
    ll.splice_after(5, c);
    ll.push_front(b, 7); // 1: (7->4->5->6)
    fr.release(c);

    ll.splice_after(b, 1); // 0: (3->1->7->4->5->6->2),  b: ()

    c = fr.request(); // 2
    assert(c == 2);
    ll.push_front(c, 8);
    ll.push_front(c, 9);
    ll.push_back(c, 10);     // 2: (9->8->10)
    ll.insert_before(8, 11); // 2: (9->11->8->10)
    ll.insert_after(8, 12);  // 2: (9->11->8->12->10)

    int d = fr.request(); // 3
    assert(d == 3);
    ll.push_back(d, 13);
    ll.push_back(d, 14);
    ll.push_back(d, 15); // 3: (13->14->15)

    // e is empty here
    int e = fr.request(); // 4
    assert(e == 4);
    ll.splice_before(14, e);
    ll.splice_after(14, e);
    ll.splice_before(13, e);
    ll.splice_after(15, e);
    ll.splice_front(d, e);
    ll.splice_back(d, e); // 3: (13->14->15)

    ll.splice_before(9, d); // 2: (13->14->15->9->11->8->12->10)
    ll.push_front(d, 16);
    ll.push_front(d, 17);
    ll.splice_back(e, d); // move 4 to 3
    ll.push_front(e, 18); // 4: (18->17->16)
    ll.splice_back(c, e); // 2: (13->14->15->9->11->8->12->10->18->17->16)
    ll.push_back(e, 19);
    ll.splice_before(10, e); // 2: (13->14->15->9->11->8->12->19->10->18->17->16)

    vector<vector<int>> fw(5), bw(5);
    for (int l = 0; l < 5; l++) {
        FOR_EACH_IN_LINKED_LIST (i, l, ll)
            fw[l].push_back(i);
        FOR_EACH_IN_LINKED_LIST_REVERSE (i, l, ll)
            bw[l].push_back(i);
    }

    // print("linked_lists + int_freelist\n"), debugh(fw), debugh(bw);
    assert(fw[0] == vector<int>({3, 1, 7, 4, 5, 6, 2}));
    assert(fw[1] == vector<int>({}));
    assert(fw[2] == vector<int>({13, 14, 15, 9, 11, 8, 12, 19, 10, 18, 17, 16}));
    assert(fw[3] == vector<int>({}));
    assert(fw[4] == vector<int>({}));
    assert(bw[0] == vector<int>({2, 6, 5, 4, 7, 1, 3}));
    assert(bw[1] == vector<int>({}));
    assert(bw[2] == vector<int>({16, 17, 18, 10, 19, 12, 8, 11, 9, 15, 14, 13}));
    assert(bw[3] == vector<int>({}));
    assert(bw[4] == vector<int>({}));

    print("OK linked_list + int_freelist\n");
}

void test_pairing_heaps() {
    constexpr int R = 5, N = 15;

    long cost[N] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28};
    pairing_int_heaps heaps(R, N, greater_container(cost));
    int u;
    (void)u;

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

    print("OK pairing_int_heaps\n");
}

void run_normal(const cost_digraph& g, int s, vector<long>& dist) {
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
            int v = g.target[e];
            long cost = dist[u] + g.cost[e];
            if (cost < dist[v]) {
                dist[v] = cost;
                Q.push({cost, v});
            }
        }
    }
}

template <typename Heap>
void run_dijkstra(const cost_digraph& g, int s, vector<long>& dist, Heap& Q) {
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
            int v = g.target[e];
            long cost = dist[u] + g.cost[e];
            if (cost < dist[v]) {
                dist[v] = cost;
                Q.push_or_improve(v);
            }
        }
    }
}

void test_heaps(int R) {
    intd distV(50, 60);
    reald density(8.0, 12.0);
    int step = 10;
    size_t dijkstra_sum = 0, pairing_heap_sum = 0, binary_heap_sum = 0;

    for (int i = 1; i <= R; i++) {
        print("\rtest heap {}...", i);
        int V = distV(mt);
        double p = density(mt) / V;
        auto g = add_costs(random_uniform_rooted_dag_connected(V, p), 100'000);

        vector<long> dist[3];
        pairing_int_heap pairing_heap(V, less_container(dist[1]));
        binary_int_heap binary_heap(V, less_container(dist[2]));

        START(dijkstra);
        for (int s = 0; s < V; s += step)
            run_normal(g, s, dist[0]);
        TIME_SHORT(dijkstra);

        START(pairing_heap);
        for (int s = 0; s < V; s += step)
            run_dijkstra(g, s, dist[1], pairing_heap);
        TIME_SHORT(pairing_heap);

        START(binary_heap);
        for (int s = 0; s < V; s += step)
            run_dijkstra(g, s, dist[2], binary_heap);
        TIME_SHORT(binary_heap);

        if (dist[0] != dist[1] || dist[0] != dist[2]) {
            print("\n-- wrong answer\n");
            debugn(dist[0]);
            debugn(dist[1]);
            debugn(dist[2]);
            break;
        }

        dijkstra_sum += time_dijkstra;
        pairing_heap_sum += time_pairing_heap;
        binary_heap_sum += time_binary_heap;
    }

    double pairingf = 100.0 * pairing_heap_sum / dijkstra_sum;
    double binaryf = 100.0 * binary_heap_sum / dijkstra_sum;

    print("\n");
    print("\t\t{:>8}ms\tstd::priority_queue\n", dijkstra_sum / 1000);
    print("\t{:4.1f}%\t{:>8}ms\tpairing_int_heap\n", pairingf, pairing_heap_sum / 1000);
    print("\t{:4.1f}%\t{:>8}ms\tbinary_int_heap\n", binaryf, binary_heap_sum / 1000);
    print("OK pairing_int_heap\n");
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_dancing_links();
    test_forward_lists();
    test_linked_lists();
    test_freelist();
    test_pairing_heaps();
    test_heaps(200);
    return 0;
}
