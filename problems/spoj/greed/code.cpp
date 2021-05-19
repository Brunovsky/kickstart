#include <bits/stdc++.h>

using namespace std;

// *****

inline namespace lib {

template <typename Container>
struct less_container {
    const Container* cont = nullptr;
    less_container() = default;
    less_container(const Container& cont) : cont(&cont) {}
    inline bool operator()(int u, int v) const {
        return tie((*cont)[u], u) < tie((*cont)[v], v);
    }
};

template <typename Compare = less<>>
struct pairing_int_heap {
    struct node_t {
        int parent = 0, child = 0, next = 0, prev = 0;
    }; // elements are shifted by 1 to allow 0 to be used as a scratchpad
    vector<node_t> node;
    int root = 0;
    Compare comp;

    explicit pairing_int_heap(int N = 0, const Compare& comp = Compare())
        : node(N + 1), comp(comp) {}

    bool empty() const { return root == 0; }
    bool contains(int u) const { return u++, node[u].parent != 0; }
    int top() const { return root - 1; }

    void push(int u) {
        assert(!contains(u)), u++;
        node[u].parent = -1;
        root = safe_meld(root, u);
    }
    int pop() {
        assert(!empty());
        int u = root;
        root = two_pass_pairing(u);
        node[root].parent = -1;
        node[u] = node_t();
        return u - 1;
    }
    void improve(int u) {
        assert(!empty() && contains(u)), u++;
        if (u != root && do_comp(u, node[u].parent)) {
            take(u), root = meld(root, u);
        }
    }
    void push_or_improve(int u) {
        if (contains(u)) {
            improve(u);
        } else {
            push(u);
        }
    }
    void adjust(int u) {
        erase(u);
        push(u);
    }
    void erase(int u) {
        assert(contains(u)), u++;
        if (u == root) {
            pop();
        } else {
            take(u);
            int v = two_pass_pairing(u);
            root = safe_meld(root, v);
            node[root].parent = -1;
            node[u] = node_t();
        }
    }
    void clear() {
        if (!empty()) {
            clear_rec(root), root = 0;
        }
    }
    void fill() {
        for (int u = 1, N = node.size() - 1; u <= N; u++) {
            if (!contains(u)) {
                push(u);
            }
        }
    }

  private:
    bool do_comp(int u, int v) const { return comp(u - 1, v - 1); }
    int meld(int u, int v) { return do_comp(u, v) ? splice(u, v) : splice(v, u); }
    int safe_meld(int u, int v) {
        if (u == 0 || v == 0 || u == v)
            return u ? u : v;
        return meld(u, v);
    }
    int splice(int u, int v) {
        node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent > 0);
        if (node[node[u].parent].child == u) {
            node[node[u].parent].child = node[u].next;
        } else {
            node[node[u].prev].next = node[u].next;
        }
        node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == 0)
            return 0;
        int u = node[n].child, v = node[u].next, w;
        while (v && node[v].next) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v) {
            w = node[v].next, u = meld(u, v), v = w;
        }
        return u;
    }
    void clear_rec(int u) {
        for (int v = node[u].child, w = node[v].next; v; v = w, w = node[v].next) {
            clear_rec(v);
        }
        node[u] = node_t();
    }
};

template <typename Cost = long, typename CostSum = Cost>
struct mincost_hungarian {
    int U = 0, V = 0, W = 0, E = 0;
    bool padded = false;
    vector<vector<pair<int, Cost>>> adj;
    vector<int> m[2];

    mincost_hungarian() = default;
    mincost_hungarian(int U, int V) : U(U), V(V), W(max(U, V)), adj(W) {}

    void add(int u, int v, Cost w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        adj[u].push_back({v, w});
    }

    void pad_complete(Cost w) {
        assert(!padded), padded = true;
        for (int v = V; v < U; v++) // edges if padding V side
            for (int u = 0; u < U; u++)
                adj[u].push_back({v, w}), E++;
        for (int u = U; u < V; u++) // edges if padding U side
            for (int v = 0; v < V; v++)
                adj[u].push_back({v, w}), E++;
    }

    void pad_reverse(Cost badw = costinf, bool guarantee = false) {
        assert(!padded), padded = true, W = U + V;
        adj.resize(W, {});
        for (int u = 0; u < U; u++)
            for (auto [v, w] : adj[u])
                adj[v + U].push_back({u + V, w});
        if (U <= V) {
            for (int v = 0; v < V; v++)
                adj[v + U].push_back({v, 0});
            for (int u = 0; u < U && guarantee; u++)
                adj[u].push_back({u + V, badw});
        } else {
            for (int u = 0; u < U; u++)
                adj[u].push_back({u + V, 0});
            for (int v = 0; v < V && guarantee; v++)
                adj[v + U].push_back({v, badw});
        }
    }

    vector<int> prev[2];
    vector<CostSum> pi[2], dist[2];
    pairing_int_heap<less_container<vector<CostSum>>> heap;
    static inline constexpr Cost costinf = numeric_limits<Cost>::max() / 3;
    static inline constexpr CostSum costsuminf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra() {
        dist[0].assign(W + 1, costsuminf);
        dist[1].assign(W, costsuminf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        for (int u = 0; u < W; u++)
            if (m[0][u] == W)
                dist[0][u] = 0, heap.push(u);

        while (!heap.empty()) {
            int u = heap.pop();
            if (u == W) {
                continue;
            }
            for (auto [v, w] : adj[u]) {
                int y = m[1][v];
                CostSum relaxed = min(dist[0][u] + w + pi[0][u] - pi[1][v], costsuminf);
                if (dist[0][y] > relaxed) {
                    dist[0][y] = relaxed, prev[0][y] = v;
                    heap.push_or_improve(y);
                }
                if (dist[1][v] > relaxed) {
                    dist[1][v] = relaxed, prev[1][v] = u;
                }
            }
        }
        reprice();
        return prev[0][W] != -1;
    }

    void reprice() {
        for (int i : {0, 1}) {
            for (int u = 0; u < W; u++) {
                pi[i][u] = min(dist[i][u] + pi[i][u], costsuminf);
            }
        }
    }

    auto path() const {
        vector<array<int, 2>> nodes;
        int v = prev[0][W];
        while (v != -1) {
            nodes.push_back({prev[1][v], v});
            v = prev[0][prev[1][v]];
        }
        return nodes;
    }

    auto mincost_max_matching(bool include_padding = false) {
        assert(U == V || padded);
        m[0].assign(W, W);
        m[1].assign(W, W);
        pi[0].assign(W, 0);
        pi[1].assign(W, 0);
        heap = pairing_int_heap<less_container<vector<CostSum>>>(W + 1, dist[0]);

        int matches = 0;
        while (matches < W && dijkstra()) {
            for (auto [u, v] : path()) {
                m[0][u] = v, m[1][v] = u;
            }
            matches++;
        }
        if (matches < W) {
            return CostSum(-1);
        }

        CostSum min_cost = 0;
        for (int u = 0; u < W; u++) {
            int v = m[0][u];
            assert(0 <= v && v < W);
            if (include_padding || (u < U && v < V)) {
                for (auto [t, w] : adj[u]) {
                    if (v == t) {
                        min_cost += w;
                        break;
                    }
                }
            }
        }
        return min_cost;
    }
};

template <typename Flow = long, typename Cost = long, typename FlowSum = Flow,
          typename CostSum = Cost>
struct mincost_edmonds_karp {
    struct Edge {
        int node[2];
        Flow cap, flow = 0;
        Cost cost;
    };
    int V, E = 0;
    vector<vector<int>> res;
    vector<Edge> edge;

    explicit mincost_edmonds_karp(int V) : V(V), res(V) {}

    void add(int u, int v, Flow capacity, Cost cost, bool bothways = false) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0 && cost >= 0);
        Flow rcapacity = bothways ? capacity : 0;
        Cost rcost = bothways ? cost : -cost;
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0, cost});
        res[v].push_back(E++), edge.push_back({{v, u}, rcapacity, 0, 0});
    }

    vector<CostSum> dist, pi;
    vector<int> prev;
    pairing_int_heap<less_container<vector<CostSum>>> heap;
    static inline constexpr Flow flowinf = numeric_limits<Flow>::max() / 2;
    static inline constexpr FlowSum flowsuminf = numeric_limits<FlowSum>::max() / 2;
    static inline constexpr CostSum costsuminf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra(int s, int t) {
        dist.assign(V, costsuminf);
        prev.assign(V, -1);
        dist[s] = 0;

        heap.push(s);

        while (!heap.empty()) {
            auto u = heap.pop();
            for (int e : res[u]) {
                int v = edge[e].node[1];
                CostSum w = min(dist[u] + pi[u] - pi[v] + edge[e].cost, costsuminf);
                if (edge[e].flow < edge[e].cap && dist[v] > w) {
                    dist[v] = w;
                    prev[v] = e;
                    heap.push_or_improve(v);
                }
            }
        }
        reprice();
        return prev[t] != -1;
    }

    void reprice() {
        for (int u = 0; u < V; u++) {
            pi[u] = min(dist[u] + pi[u], costsuminf);
        }
    }

    auto path(int v) const {
        vector<int> path;
        while (prev[v] != -1) {
            path.push_back(prev[v]);
            v = edge[prev[v]].node[0];
        }
        return path;
    }

    pair<FlowSum, CostSum> mincost_flow(int s, int t, FlowSum F = flowsuminf) {
        pi.assign(V, 0);
        heap = pairing_int_heap<less_container<vector<CostSum>>>(V, dist);

        FlowSum sflow = 0;
        while (sflow < F && dijkstra(s, t)) {
            auto augmenting_path = path(t);
            Flow df = min(F - sflow, FlowSum(flowinf));
            for (int e : augmenting_path) {
                df = min(df, edge[e].cap - edge[e].flow);
            }
            sflow += df;
            for (int e : augmenting_path) {
                edge[e].flow += df;
                edge[e ^ 1].flow -= df;
            }
        }
        CostSum scost = 0;
        for (int e = 0; e < E; e += 2) {
            scost += FlowSum(edge[e].flow) * CostSum(edge[e].cost);
        }
        return {sflow, scost};
    }

    void clear_flow() {
        for (int e = 0; e < E; e++) {
            edge[e].flow = 0;
        }
    }

    Flow get_flow(int e) const { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return dist[u] < flowsuminf; }
};

} // namespace lib

constexpr int MAXV = 500, inf = INT_MAX / 2;

auto solve_mincost_flow() {
    int V;
    cin >> V;
    vector<int> cards(V);
    vector<int> cnt(V);
    for (int i = 0; i < V; i++) {
        cin >> cards[i], cards[i]--;
        cnt[cards[i]]++;
    }

    int E;
    cin >> E;
    vector<vector<int>> adj(V);
    for (int e = 0; e < E; e++) {
        int u, v;
        cin >> u >> v, u--, v--;
        if (u != v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    int s = V, t = V + 1;
    mincost_edmonds_karp<int, int> mcf(V + 2);

    for (int u = 0; u < V; u++) {
        if (cnt[u] > 0) {
            mcf.add(s, u, cnt[u], 0, false);
        }
        for (int v : adj[u]) {
            if (u < v) {
                mcf.add(u, v, V, 1, true);
            }
        }
        mcf.add(u, t, 1, 0, false);
    }

    auto [sflow, scost] = mcf.mincost_flow(s, t, V);
    assert(sflow == V);
    return scost;
}

auto solve_mincost_hungarian() {
    int V;
    cin >> V;
    vector<int> cards(V);
    vector<int> cnt(V);
    for (int i = 0; i < V; i++) {
        cin >> cards[i], cards[i]--;
        cnt[cards[i]]++;
    }

    int E;
    cin >> E;
    vector<vector<int>> adj(V);
    for (int e = 0; e < E; e++) {
        int u, v;
        cin >> u >> v, u--, v--;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<vector<int>> dist(V, vector<int>(V, inf));
    vector<int> bfs(V);
    for (int s = 0; s < V; s++) {
        dist[s][s] = 0;
        bfs[0] = s;
        int i = 0, S = 1;
        while (i < S) {
            int u = bfs[i++];
            for (int v : adj[u]) {
                if (dist[s][v] == inf) {
                    dist[s][v] = dist[s][u] + 1;
                    bfs[S++] = v;
                }
            }
        }
    }

    mincost_hungarian<int> mch(V, V);

    for (int u = 0; u < V; u++) {
        for (int v = 0; v < V; v++) {
            if (dist[cards[u]][v] < inf) {
                mch.add(u, v, dist[cards[u]][v]);
            }
        }
    }

    return mch.mincost_max_matching();
}

// *****

int main() {
    ios::sync_with_stdio(false);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << solve_mincost_flow() << endl;
    }
    return 0;
}
