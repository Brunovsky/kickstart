#pragma GCC optimize "O3"
#include <bits/stdc++.h>

using namespace std;

#define long int64_t

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

template <typename Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

template <typename T, int NDIMS>
struct tensor {
    static_assert(NDIMS >= 0, "NDIMS must be nonnegative");

  protected:
    std::array<int, NDIMS> shape;
    std::array<int, NDIMS> strides;
    int len;
    T* data;

  public:
    tensor() : shape{0}, strides{0}, len(0), data(nullptr) {}

    explicit tensor(std::array<int, NDIMS> shape_, const T& t = T()) {
        shape = shape_;
        strides[NDIMS - 1] = 1;
        for (int i = NDIMS - 1; i > 0; i--) {
            strides[i - 1] = strides[i] * shape[i];
        }
        len = strides[0] * shape[0];
        data = new T[len];
        std::fill(data, data + len, t);
    }

    tensor(const tensor& o)
        : shape(o.shape), strides(o.strides), len(o.len), data(new T[len]) {
        for (int i = 0; i < len; i++) {
            data[i] = o.data[i];
        }
    }

    tensor& operator=(tensor&& o) noexcept {
        using std::swap;
        swap(shape, o.shape);
        swap(strides, o.strides);
        swap(len, o.len);
        swap(data, o.data);
        return *this;
    }
    tensor(tensor&& o) : tensor() { *this = std::move(o); }
    tensor& operator=(const tensor& o) { return *this = tensor(o); }
    ~tensor() { delete[] data; }

    auto size() const { return shape; }

  protected:
    int flatten_index(std::array<int, NDIMS> idx) const {
        int res = 0;
        for (int i = 0; i < NDIMS; i++) {
            res += idx[i] * strides[i];
        }
        return res;
    }
    int flatten_index_checked(std::array<int, NDIMS> idx) const {
        int res = 0;
        for (int i = 0; i < NDIMS; i++) {
            assert(0 <= idx[i] && idx[i] < shape[i]);
            res += idx[i] * strides[i];
        }
        return res;
    }

  public:
    T& operator[](std::array<int, NDIMS> idx) const { return data[flatten_index(idx)]; }
    T& at(std::array<int, NDIMS> idx) const { return data[flatten_index_checked(idx)]; }
};

template <typename T>
struct min_rmq {
    vector<vector<T>> jmp;
    static constexpr int bits = CHAR_BIT * sizeof(int) - 1;

    min_rmq() = default;
    explicit min_rmq(const vector<T>& v) : jmp(1, v) {
        for (unsigned len = 1, k = 1; len * 2 <= v.size(); len *= 2, ++k) {
            jmp.emplace_back(v.size() - len * 2 + 1);
            for (unsigned j = 0; j < jmp[k].size(); j++) {
                const auto& l = jmp[k - 1][j];
                const auto& r = jmp[k - 1][j + len];
                jmp[k][j] = min(l, r);
            }
        }
    }

    T query(int a, int b) const /* [a,b) */ {
        assert(a < b); // or return inf if a == b
        int dep = bits - __builtin_clz(b - a);
        const auto& l = jmp[dep][a];
        const auto& r = jmp[dep][b - (1 << dep)];
        return min(l, r);
    }
};

int next_two(int32_t N) { return N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0; }
void setmin(int& a, int b) { a = min(a, b); }
constexpr int inf = INT_MAX;

int main() {
    ios::sync_with_stdio(false), cout.tie(nullptr), cin.tie(nullptr);
    int N, root;
    cin >> N >> root;

    vector<int> arr(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> arr[i];
    }

    vector<vector<int>> tree(N + 1);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    vector<array<int, 2>> index(N + 1);
    vector<int> depth(N + 1);
    vector<int> deepest(N + 1);
    depth[0] = -1;
    int timer = 0;

    y_combinator([&](auto self, int u, int p) -> void {
        index[u][0] = timer++;
        depth[u] = depth[p] + 1;
        deepest[u] = depth[u];
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
                deepest[u] = max(deepest[u], deepest[v]);
            }
        }
        index[u][1] = timer;
    })(root, 0);

    int D = 1 + deepest[root];
    int B = 1;
    while (B < D)
        B <<= 1;

    vector<array<int, 2>> reverse_index(N + 1);
    for (int u = 1; u <= N; u++) {
        reverse_index[index[u][0]][0] = u;
        reverse_index[index[u][1]][1] = u;
    }

    // Compute euler tours by depth
    vector<vector<int>> on_depth(D);

    for (int i = 0; i < N; i++) {
        int u = reverse_index[i][0];
        on_depth[depth[u]].push_back(i);
    }

    // Aggregate depths in segrmq, compute ranges first
    vector<min_rmq<int>> segrmq(2 * D);
    vector<array<int, 2>> segrange(2 * D);
    vector<vector<int>> on_block(2 * D);

    for (int d = 0; d < D; d++) {
        segrange[d + D] = {d, d};
        on_block[d + D] = on_depth[d];

        int O = on_depth[d].size();
        vector<int> depth_arr(O);
        for (int j = 0; j < O; j++) {
            depth_arr[j] = arr[reverse_index[on_depth[d][j]][0]];
        }
        segrmq[d + D] = min_rmq<int>(depth_arr);
    }

    rotate(begin(segrange) + D, begin(segrange) + (3 * D - B), end(segrange));
    rotate(begin(on_block) + D, begin(on_block) + (3 * D - B), end(on_block));
    rotate(begin(segrmq) + D, begin(segrmq) + (3 * D - B), end(segrmq));

    // Pushup aggregates
    for (int i = D - 1; i >= 1; i--) {
        auto& on = on_block[i];
        const auto& onL = on_block[i << 1];
        const auto& onR = on_block[i << 1 | 1];
        int O = onL.size() + onR.size();

        on.resize(O);
        merge(begin(onL), end(onL), begin(onR), end(onR), begin(on));

        vector<int> block_arr(O);
        for (int j = 0; j < O; j++) {
            block_arr[j] = arr[reverse_index[on[j]][0]];
        }
        segrange[i] = {segrange[i << 1][0], segrange[i << 1 | 1][1]};
        segrmq[i] = min_rmq<int>(block_arr);
    }

    // Find bounds on specific depth for subtree of u
    auto query_bounds = [&](const auto& on, int u) -> array<int, 2> {
        int O = on.size();
        auto [in, out] = index[u];

        auto liter = lower_bound(begin(on), end(on), in);
        auto riter = upper_bound(rbegin(on), rend(on), out, greater<int>{});
        assert(liter != end(on) && riter != rend(on));

        int l = liter - begin(on);
        int r = O - 1 - (riter - rbegin(on));
        return {l, r};
    };

    auto query_segtree = y_combinator([&](auto self, int i, int u, int d) -> int {
        // querying range [depth[u], d]
        assert(depth[u] <= d && d <= deepest[u] && i < 2 * D);
        auto [min_depth, max_depth] = segrange[i];
        if (max_depth < depth[u] || d < min_depth) {
            return inf;
        }
        if (depth[u] <= min_depth && max_depth <= d) {
            auto [l, r] = query_bounds(on_block[i], u);
            return segrmq[i].query(l, r + 1);
        }
        return min(self(i << 1, u, d), self(i << 1 | 1, u, d));
    });

    int Q, last = 0;
    cin >> Q;

    while (Q--) {
        int p, q;
        cin >> p >> q;
        int u = (p + last) % N + 1;
        int d = depth[u] + (q + last) % N;
        // int u = p, d = q + depth[u];
        setmin(d, deepest[u]);

        last = query_segtree(1, u, d);
        cout << last << endl;
    }

    return 0;
}
