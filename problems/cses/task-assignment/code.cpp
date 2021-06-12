#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

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
struct binary_int_heap {
    vector<int> c, id;
    Compare comp;

    explicit binary_int_heap(int N = 0, const Compare& comp = Compare())
        : c(0, 0), id(N, -1), comp(comp) {}

    bool empty() const { return c.empty(); }
    size_t size() const { return c.size(); }
    bool contains(int u) const { return id[u] != -1; }
    int top() const {
        assert(!empty());
        return c[0];
    }
    void push(int u) {
        assert(!contains(u));
        id[u] = c.size(), c.push_back(u);
        heapify_up(id[u]);
    }
    int pop() {
        assert(!empty());
        int u = c[0];
        c[0] = c.back();
        id[c[0]] = 0, id[u] = -1;
        c.pop_back();
        heapify_down(0);
        return u;
    }
    void improve(int u) { assert(contains(u)), heapify_up(id[u]); }
    void decline(int u) { assert(contains(u)), heapify_down(id[u]); }
    void push_or_improve(int u) { contains(u) ? improve(u) : push(u); }
    void push_or_decline(int u) { contains(u) ? decline(u) : push(u); }
    void clear() {
        for (int u : c)
            id[u] = -1;
        c.clear();
    }
    void fill() {
        for (int u = 0, N = id.size(); u < N; u++) {
            if (!contains(u)) {
                push(u);
            }
        }
    }

  private:
    static int parent(int i) { return (i - 1) >> 1; }
    static int child(int i) { return i << 1 | 1; }
    void exchange(int i, int j) { swap(id[c[i]], id[c[j]]), swap(c[i], c[j]); }
    void heapify_up(int i) {
        while (i > 0 && comp(c[i], c[parent(i)])) { // while c[i] < c[parent(i)]
            exchange(i, parent(i)), i = parent(i);
        }
    }
    void heapify_down(int i) {
        int k, S = c.size();
        while ((k = child(i)) < S) {
            if (k + 1 < S && !comp(c[k], c[k + 1])) // if c[rchild(i)] <= c[lchild(i)]
                k++;
            if (!comp(c[k], c[i])) // break if c[i] <= c[minchild(i)]
                break;
            exchange(i, k), i = k;
        }
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
    binary_int_heap<less_container<vector<CostSum>>> heap;
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
        heap = binary_int_heap<less_container<vector<CostSum>>>(W + 1, dist[0]);

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

int main() {
    ios::sync_with_stdio(false);
    int N;
    cin >> N;
    mincost_hungarian<int> mcm(N, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int cost;
            cin >> cost;
            mcm.add(i, j, cost);
        }
    }
    cout << mcm.mincost_max_matching() << endl;
    for (int i = 0; i < N; i++) {
        cout << i + 1 << ' ' << mcm.m[0][i] + 1 << endl;
    }
    return 0;
}
