#include "../integer_data_structures.hpp"

#include "../debug_print.hpp"
#include "../graph_convert.hpp"
#include "../graph_generator.hpp"
#include "../shortest_path.hpp"
#include "chrono.hpp"

// *****

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

    print("forward_lists\n");
    debugh(seen);
    assert(seen[0] == vector<int>({}));
    assert(seen[1] == vector<int>({2}));
    assert(seen[2] == vector<int>({0, 9, 7, 4, 3}));
    assert(seen[3] == vector<int>({6, 1, 5}));
    assert(seen[4] == vector<int>({8}));
    assert(seen[5] == vector<int>({}));
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

    print("linked_lists\n");
    debugh(fw), debugh(bw);
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

    print("linked_lists + int_freelist\n");
    debugh(fw), debugh(bw);
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
}

void test_int_forest() {
    int N = 15;
    linked_int_forest f(N);
    f.init(0);
    f.init(1);
    f.init(2); // N(0,1,2)
    assert(f.S == 3);

    f.splice_back(1, 3);
    f.splice_back(1, 4); // N(0,1(3,4),2)
    f.splice_back(2, 5);
    f.splice_back(2, 6); // N(0,1(3,4),2(5,6))
    assert(f.S == 7);

    f.splice_back(0, 2); // N(0(2(5,6)),1(3,4))
    assert(f.S == 7);

    f.splice_before(6, 7);
    f.splice_after(3, 8); // N(0(2(5,7,6)),1(3,8,4))
    assert(f.S == 9);

    f.clear(5);
    assert(f.S == 8);

    f.init(5);
    f.init(9); // N(0(2(7,6)),1(3,8,4),5,9)
    assert(f.S == 10);

    f.splice_back(9, 10);
    f.splice_back(9, 11);
    f.splice_back(9, 12); // N(0(2(7,6)),1(3,8,4),5,9(10,11,12))
    f.splice_back(2, 13); // N(0(2(7,6,13)),1(3,8,4),5,9(10,11,12))
    assert(f.S == 14);

    f.init(6);              // N(0(2(7,13)),1(3,8,4),5,9(10,11,12),6)
    f.splice_before(13, 9); // N(0(2(7,9(10,11,12),13)),1(3,8,4),5,6)
    f.splice_back(0, 14);   // N(0(2(7,9(10,11,12),13),14),1(3,8,4),5,6)
    f.splice_back(8, 9);    // N(0(2(7,13),14),1(8(9(10,11,12)),3,4),5,6)
    assert(f.S == 15);

    vector<vector<int>> par(N + 1), child(N + 1);

    for (int n = 0; n <= N; n++) {
        FOR_EACH_CHILD_INT_TREE (i, n, f)
            child[n].push_back(i);
        if (n < N)
            FOR_EACH_ANCESTOR_INT_TREE (i, n, f)
                par[n].push_back(i);
    }

    print("int_forest\n");
    diota("u", f.S);
    debugn(f.parent);
    debugn(f.next);
    debugn(f.prev);
    debugh(par), debugh(child);
}

dijkstra to_dijkstra(const cost_digraph& g) {
    dijkstra runner(g.V);
    for (int e = 0; e < g.E; e++) {
        runner.add(g.source[e], g.target[e], g.cost[e]);
    }
    return runner;
}

template <typename Heap>
void run_dijkstra(const cost_digraph& g, int s, vector<long>& dist, vector<int>& prev,
                  Heap& Q) {
    static constexpr long inf = LONG_MAX / 2;
    int V = g.V;
    dist.assign(V, inf);
    prev.assign(V, -1);
    dist[s] = 0;
    prev[s] = s;

    vector<bool> vis(V, false);
    assert(Q.empty());
    Q.push(s);

    while (!Q.empty()) {
        int u = Q.pop();
        assert(!vis[u]);
        vis[u] = true;
        for (auto e : g.adj[u]) {
            int v = g.target[e];
            long cost = dist[u] + g.cost[e];
            if (!vis[v] && cost < dist[v]) {
                dist[v] = cost;
                prev[v] = u;
                Q.push_or_improve(v);
            }
        }
    }
}

template <template <typename...> typename Heap>
void test_heap(const string& name, int R) {
    intd distV(320, 400);
    reald density(90.0, 150.0);
    size_t dijkstra_sum = 0, heap_sum = 0;

    for (int i = 1; i <= R; i++) {
        print("\rtest heap {} {}...", name, i);
        int V = distV(mt);
        double p = density(mt) / V;
        auto g = add_costs(random_uniform_rooted_dag_connected(V, p), 10);
        dijkstra runner = to_dijkstra(g);

        vector<long> dist;
        vector<int> prev;
        auto cmp = [&](int u, int v) {
            return dist[u] < dist[v];
        };
        using heap_t = Heap<decltype(cmp)>;
        heap_t Q(V, cmp);

        START(dijkstra);
        for (int s = 0; s < V; s++)
            runner.compute(s);
        TIME_SHORT(dijkstra);

        START(heap);
        for (int s = 0; s < V; s++)
            run_dijkstra(g, s, dist, prev, Q);
        TIME_SHORT(heap);

        if (runner.dist != dist) {
            print("\n-- wrong answer\n");
            debugn(dist);
            debugn(runner.dist);
            break;
        }

        dijkstra_sum += time_dijkstra;
        heap_sum += time_heap;
    }

    print("\n");
    print(" {:>8}ms dijkstra\n", dijkstra_sum / 1000);
    print(" {:>8}ms heap\n", heap_sum / 1000);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test_forward_lists();
    test_linked_lists();
    test_freelist();
    test_int_forest();
    test_heap<pairing_int_heap>("pairing_int_heap", 100);
    test_heap<binary_int_heap>("binary_int_heap", 100);
    return 0;
}
