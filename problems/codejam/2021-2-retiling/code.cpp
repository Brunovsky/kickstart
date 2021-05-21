#include <bits/stdc++.h>

using namespace std;

// *****

inline namespace {

template <typename Container>
struct less_container {
    const Container& cont;
    less_container(const Container& cont) : cont(cont) {}
    inline bool operator()(int u, int v) const {
        return tie(cont[u], u) < tie(cont[v], v);
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

/**
 * Min-cost maximum bipartite matching (hungarian, dijkstra-based)
 * Based on: https://web.stanford.edu/class/cs261/min_cost_bipartite_matching.pdf
 * Complexity: O(EW log E), W=max(U,V).
 *
 * Need pairing_int_heap for the internal dijkstra.
 *
 * If the bipartite graph is not balanced (U != V) it will be padded with infinite weight
 * edges. A perfect matching must exist on the padded graph, otherwise the answer risks
 * being bogus. To obtain a min-cost non-perfect matching of arbitrary size use reduction
 * to mincost flow instead.
 */
template <typename Cost = long, typename CostSum = Cost>
struct mincost_hungarian {
    using edges_t = vector<array<int, 2>>;
    int U = 0, V = 0, W = 0, E = 0;
    vector<vector<pair<int, Cost>>> adj;
    vector<int> m[2];

    explicit mincost_hungarian(int U, int V) : U(U), V(V), W(max(U, V)), adj(W) {}

    void pad(Cost w) {
        for (int v = V; v < U; v++) // edges if padding V side
            for (int u = 0; u < U; u++)
                adj[u].push_back({v, w}), E++;
        for (int u = U; u < V; u++) // edges if padding U side
            for (int v = 0; v < V; v++)
                adj[u].push_back({v, w}), E++;
    }

    void add(int u, int v, Cost w) {
        assert(0 <= u && u < U && 0 <= v && v < V && w >= 0);
        adj[u].push_back({v, w}), E++;
    }

    vector<int> prev[2];
    vector<CostSum> pi[2], dist[2];
    static inline constexpr CostSum inf = numeric_limits<CostSum>::max() / 3;

    bool dijkstra() {
        dist[0].assign(W + 1, inf);
        dist[1].assign(W, inf);
        prev[0].assign(W + 1, -1);
        prev[1].assign(W, -1);

        vector<bool> vis(W, false);
        pairing_int_heap<less_container<vector<CostSum>>> Q(W + 1, dist[0]);

        for (int u = 0; u < W; u++)
            if (m[0][u] == W)
                dist[0][u] = 0, Q.push(u);

        while (!Q.empty()) {
            int u = Q.pop();
            if (u == W || vis[u]) {
                continue;
            }
            vis[u] = true;
            for (auto [v, w] : adj[u]) {
                int y = m[1][v];
                w = min(dist[0][u] + w + pi[0][u] - pi[1][v], inf);
                if (dist[0][y] > w) {
                    dist[0][y] = w, prev[0][y] = v;
                    Q.push_or_improve(y);
                }
                if (dist[1][v] > w) {
                    dist[1][v] = w, prev[1][v] = u;
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
        edges_t nodes;
        int v = prev[0][W];
        while (v != -1) {
            nodes.push_back({prev[1][v], v});
            v = prev[0][prev[1][v]];
        }
        return nodes;
    }

    auto mincost_max_matching() {
        m[0].assign(W, W);
        m[1].assign(W, W);
        pi[0].assign(W, 0);
        pi[1].assign(W, 0);

        int matchings = 0;
        while (matchings < W && dijkstra()) {
            for (auto [u, v] : path()) {
                m[0][u] = v, m[1][v] = u;
            }
            matchings++;
        }
        if (matchings < W)
            return CostSum(-1);

        CostSum min_cost = 0;
        for (int u = 0; u < W; u++) {
            if (int v = m[0][u]; v < W) {
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

} // namespace

auto solve() {
    int R, C;
    long FLIP, SWAP;
    cin >> R >> C >> FLIP >> SWAP;

    vector<string> a(R), b(R);
    for (int r = 0; r < R; r++) {
        cin >> a[r];
    }
    for (int r = 0; r < R; r++) {
        cin >> b[r];
    }

    vector<array<int, 2>> am, bm;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            if (a[r][c] == 'M') {
                am.push_back({r, c});
            }
            if (b[r][c] == 'M') {
                bm.push_back({r, c});
            }
        }
    }

    int U = am.size(), V = bm.size();
    mincost_hungarian<long, long> mch(U, V);

    for (int u = 0; u < U; u++) {
        for (int v = 0; v < V; v++) {
            auto [ar, ac] = am[u];
            auto [br, bc] = bm[v];
            long dist = abs(ar - br) + abs(ac - bc);
            mch.add(u, v, min(2 * FLIP, SWAP * dist));
        }
    }

    mch.pad(FLIP);
    long ans = mch.mincost_max_matching();
    return ans;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
