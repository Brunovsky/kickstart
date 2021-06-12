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

    void add(int u, int v, Flow capacity, Cost cost) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && capacity > 0 && cost >= 0);
        res[u].push_back(E++), edge.push_back({{u, v}, capacity, 0, cost});
        res[v].push_back(E++), edge.push_back({{v, u}, 0, 0, -cost});
    }

    vector<CostSum> dist, pi;
    vector<int> prev;
    binary_int_heap<less_container<vector<CostSum>>> heap;
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
        heap = binary_int_heap<less_container<vector<CostSum>>>(V, dist);

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
        return make_pair(sflow, scost);
    }

    void clear_flow() {
        for (int e = 0; e < E; e++) {
            edge[e].flow = 0;
        }
    }

    Flow& get_flow(int e) { return edge[2 * e].flow; }
    bool left_of_mincut(int u) const { return dist[u] < costsuminf; }
};

int main() {
    ios::sync_with_stdio(false);
    int N, M, K;
    cin >> N >> M >> K;
    int s = 0, t = N - 1;
    mincost_edmonds_karp<int, int> mcmf(N);

    vector<vector<pair<int, int>>> out(N);

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        out[u].push_back({i, v});
        mcmf.add(u, v, 1, 1);
    }

    auto [flow, cost] = mcmf.mincost_flow(s, t, K);
    if (flow < K) {
        cout << -1 << endl;
        return 0;
    }
    cout << cost << endl;
    // Build disjoint paths, doesn't matter which we pick
    while (K--) {
        int u = s;
        vector<int> path = {u};
        while (u != t) {
            for (auto [e, v] : out[u]) {
                if (mcmf.get_flow(e) > 0) {
                    mcmf.get_flow(e)--;
                    path.push_back(u = v);
                    break;
                }
            }
        }
        int P = path.size();
        cout << P << endl;
        for (int i = 0; i < P; i++) {
            cout << path[i] + 1 << " \n"[i + 1 == P];
        }
    }
    return 0;
}
