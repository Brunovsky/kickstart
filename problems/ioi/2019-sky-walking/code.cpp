#include <bits/stdc++.h>

using namespace std;

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
namespace gnu = __gnu_pbds;

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

struct Hasher {
    template <typename Container>
    size_t operator()(const Container& vec) const noexcept {
        using std::hash;
        hash<typename Container::value_type> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (const auto& n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    template <typename U, typename V>
    size_t operator()(const pair<U, V>& p) const noexcept {
        using std::hash;
        size_t a = hash<U>{}(p.first), b = hash<V>{}(p.second);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <typename U>
    size_t operator()(const array<U, 2>& p) const noexcept {
        using std::hash;
        hash<U> hasher;
        size_t a = hasher(p[0]), b = hasher(p[1]);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <int i, typename Tuple>
    size_t tuple_compute(const Tuple& tuple) const noexcept {
        if constexpr (i == std::tuple_size_v<Tuple>) {
            return std::tuple_size_v<Tuple>;
        } else {
            using std::hash;
            hash<std::tuple_element_t<i, Tuple>> hasher;
            size_t seed = tuple_compute<i + 1, Tuple>(tuple);
            size_t h = hasher(std::get<i>(tuple));
            return seed ^ (h + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    }
    template <typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const noexcept {
        return tuple_compute<0, tuple<Ts...>>(t);
    }
    size_t operator()(__uint128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return this->operator()(*arr);
    }
    size_t operator()(__int128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return this->operator()(*arr);
    }
};

} // namespace lib

namespace std {

template <typename T, size_t N>
struct hash<array<T, N>> : Hasher {};
template <typename T>
struct hash<vector<T>> : Hasher {};
template <typename U, typename V>
struct hash<pair<U, V>> : Hasher {};
template <typename... Ts>
struct hash<tuple<Ts...>> : Hasher {};
template <>
struct hash<__int128_t> : Hasher {};
template <>
struct hash<__uint128_t> : Hasher {};

} // namespace std

template <typename K, typename V>
using hash_map = gnu::gp_hash_table<K, V, Hasher>;

long long min_distance(vector<int> tower_x, vector<int> tower_h, vector<int> bridge_l,
                       vector<int> bridge_r, vector<int> bridge_y, int sx, int tx) {
    int N = tower_x.size();
    int M = bridge_y.size();

    hash_map<pair<int, int>, int> idmap;
    auto get_id = [&idmap](int x, int y) {
        return idmap.insert({pair<int, int>{x, y}, idmap.size()}).first->second;
    };

    int s = get_id(tower_x[sx], 0);
    int t = get_id(tower_x[tx], 0);
    vector<array<int, 3>> edges;

    set<int> alive;
    for (int i = 0; i < N; i++) {
        alive.insert(i);
    }

    deque<int> towers_by_height(N);
    iota(begin(towers_by_height), end(towers_by_height), 0);
    sort(begin(towers_by_height), end(towers_by_height),
         [&](int u, int v) { return tower_h[u] < tower_h[v]; });

    vector<int> bridges_by_height(M);
    iota(begin(bridges_by_height), end(bridges_by_height), 0);
    sort(begin(bridges_by_height), end(bridges_by_height),
         [&](int u, int v) { return bridge_y[u] < bridge_y[v]; });

    vector<set<int>> interesting(N);

    for (int b = 0; b < M; b++) {
        int bridge = bridges_by_height[b];
        int l = bridge_l[bridge];
        int r = bridge_r[bridge];
        int y = bridge_y[bridge];

        while (tower_h[towers_by_height[0]] < y) {
            alive.erase(towers_by_height[0]);
            towers_by_height.pop_front();
        }

        auto start = alive.find(l);
        auto end = next(alive.find(r));
        vector<int> above(start, end);
        int A = above.size();

        for (int k = 0; k + 1 < A; k++) {
            int i = above[k], j = above[k + 1];
            int x0 = tower_x[i], x1 = tower_x[j];
            edges.push_back({get_id(x0, y), get_id(x1, y), x1 - x0});
            edges.push_back({get_id(x1, y), get_id(x0, y), x1 - x0});
        }
        for (int i : above) {
            interesting[i].insert(y);
        }
    }

    for (int i = 0; i < N; i++) {
        if (i == sx || i == tx) {
            interesting[i].insert(0);
        }
        vector<int> points(begin(interesting[i]), end(interesting[i]));
        int P = points.size();
        int x = tower_x[i];

        for (int j = 0; j + 1 < P; j++) {
            int y0 = points[j], y1 = points[j + 1];
            edges.push_back({get_id(x, y0), get_id(x, y1), y1 - y0});
            edges.push_back({get_id(x, y1), get_id(x, y0), y1 - y0});
        }
    }

    int V = idmap.size();
    vector<vector<pair<int, int>>> adj(V);
    for (auto [u, v, w] : edges) {
        adj[u].push_back({v, w});
    }

    using CostSum = long long;
    constexpr CostSum inf = numeric_limits<CostSum>::max() / 2;

    vector<CostSum> dist(V, inf);
    dist[s] = 0;

    using min_heap = binary_int_heap<less_container<vector<CostSum>>>;
    min_heap heap(V, dist);
    heap.push(s);

    do {
        int u = heap.pop();

        for (auto [v, w] : adj[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                heap.push_or_improve(v);
            }
        }
    } while (!heap.empty() && heap.top() != t);

    return dist[t] == inf ? -1 : dist[t];
}

// *****

int main() {
    auto ans0 = min_distance({0, 3, 5, 7, 10, 12, 14}, {8, 7, 9, 7, 6, 6, 9},
                             {0, 0, 0, 2, 2, 3, 4}, {1, 2, 6, 3, 6, 4, 6},
                             {1, 6, 8, 1, 7, 2, 5}, 1, 5);

    auto ans1 = min_distance({0, 4, 5, 6, 9}, {6, 6, 6, 6, 6}, {3, 1, 0}, {4, 3, 2},
                             {1, 3, 6}, 0, 4);

    cout << "ans 0: " << ans0 << endl;
    cout << "ans 1: " << ans1 << endl;
    return 0;
}
