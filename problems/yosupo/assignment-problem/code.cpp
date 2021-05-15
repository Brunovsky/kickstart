#include <bits/stdc++.h>

using namespace std;

// *****

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
        root = empty() ? u : meld(root, u);
    }
    int pop() {
        assert(!empty());
        int u = root;
        root = two_pass_pairing(u);
        node[root].parent = -1, node[u] = node_t();
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
            root = v ? meld(root, v) : root;
            node[root].parent = -1, node[u] = node_t();
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

    void pad_reverse() {
        assert(!padded), padded = true, W = U + V;
        adj.resize(W, {});
        for (int u = 0; u < U; u++)
            for (auto [v, w] : adj[u])
                adj[v + U].push_back({u + V, w});
        if (U <= V) {
            for (int v = 0; v < V; v++)
                adj[v + U].push_back({v, 0});
            for (int u = 0; u < U; u++)
                adj[u].push_back({u + V, cinf});
        } else {
            for (int u = 0; u < U; u++)
                adj[u].push_back({u + V, 0});
            for (int v = 0; v < V; v++)
                adj[v + U].push_back({v, cinf});
        }
    }

    vector<int> prev[2];
    vector<CostSum> pi[2], dist[2];
    pairing_int_heap<less_container<vector<CostSum>>> Q;
    static inline constexpr Cost cinf = numeric_limits<Cost>::max() / 3;
    static inline constexpr CostSum inf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra() {
        dist[0].assign(W + 1, inf);
        dist[1].assign(W, inf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        for (int u = 0; u < W; u++)
            if (m[0][u] == W)
                dist[0][u] = 0, Q.push(u);

        while (!Q.empty()) {
            int u = Q.pop();
            if (u == W) {
                continue;
            }
            for (auto [v, w] : adj[u]) {
                int y = m[1][v];
                CostSum relaxed = min(dist[0][u] + w + pi[0][u] - pi[1][v], inf);
                if (dist[0][y] > relaxed) {
                    dist[0][y] = relaxed, prev[0][y] = v;
                    Q.push_or_improve(y);
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
                pi[i][u] = min(dist[i][u] + pi[i][u], inf);
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
        Q = pairing_int_heap<less_container<vector<CostSum>>>(W + 1, dist[0]);

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

#define MAXA 1'000'000'000L

auto solve() {
    int N;
    cin >> N;
    mincost_hungarian<long, long> mch(N, N);
    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            long a;
            cin >> a;
            a += MAXA;
            mch.add(u, v, a);
        }
    }

    long ans = mch.mincost_max_matching();
    ans -= N * MAXA;
    cout << ans << endl;

    for (int u = 0; u < N; u++) {
        cout << mch.m[0][u] << " \n"[u + 1 == N];
    }
}

// *****

int main() {
    ios::sync_with_stdio(false);
    solve();
    return 0;
}
