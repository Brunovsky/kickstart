#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <typename Fun>
class y_combinator_result {
    Fun fun_;

  public:
    template <typename T>
    explicit y_combinator_result(T&& fun) : fun_(std::forward<T>(fun)) {}

    template <typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return fun_(std::ref(*this), std::forward<Args>(args)...);
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
};

namespace std {

template <typename T, size_t N>
struct hash<array<T, N>> : Hasher {};
template <typename T>
struct hash<vector<T>> : Hasher {};
template <typename U, typename V>
struct hash<pair<U, V>> : Hasher {};
template <typename... Ts>
struct hash<tuple<Ts...>> : Hasher {};

} // namespace std

template <typename Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

auto hash_unrooted_tree(int V, const vector<array<int, 2>>& g) {
    static hash<vector<size_t>> vec_hasher;
    static hash<pair<size_t, size_t>> pair_hasher;

    vector<vector<int>> adj(V);
    for (auto [u, v] : g) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    vector<int> subsize(V), centroids;

    auto dfs_centroid = y_combinator([&](auto self, int u, int p) -> void {
        subsize[u] = 1;
        bool is = true;
        for (int v : adj[u]) {
            if (v != p) {
                self(v, u);
                subsize[u] += subsize[v];
                is &= subsize[v] <= V / 2;
            }
        }
        if (is && V - subsize[u] <= V / 2)
            centroids.push_back(u);
    });

    dfs_centroid(0, -1);
    int C = centroids.size();
    assert(C == 1 || C == 2);

    auto hash_subtree = y_combinator([&](auto self, int u, int p) -> size_t {
        vector<size_t> hashes;
        subsize[u] = 1;
        for (int v : adj[u]) {
            if (v != p) {
                hashes.push_back(self(v, u));
                subsize[u] += subsize[v];
            }
        }
        sort(begin(hashes), end(hashes));
        hashes.push_back(subsize[u]);
        return vec_hasher(hashes);
    });

    if (C == 1) {
        return hash_subtree(centroids[0], -1);
    } else {
        auto hu = hash_subtree(centroids[0], centroids[1]);
        auto hv = hash_subtree(centroids[1], centroids[0]);
        pair<size_t, size_t> phash = minmax(hu, hv);
        return pair_hasher(phash);
    }
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        vector<array<int, 2>> a(N - 1), b(N - 1);
        for (auto& [u, v] : a) {
            cin >> u >> v, u--, v--;
        }
        for (auto& [u, v] : b) {
            cin >> u >> v, u--, v--;
        }
        if (hash_unrooted_tree(N, a) == hash_unrooted_tree(N, b)) {
            cout << "YES\n";
        } else {
            cout << "NO\n";
        }
    }
    return 0;
}
