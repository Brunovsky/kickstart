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

auto build_euler_tour_tree_index(const vector<vector<int>>& tree, int root) {
    int V = tree.size();
    vector<array<int, 2>> index(V);
    int timer = 0;

    auto dfs = y_combinator([&](auto self, int u, int p) -> void {
        index[u][0] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
            }
        }
        index[u][1] = timer++;
    });

    dfs(root, -1);
    return index;
}

template <typename T>
struct min_rmq {
    vector<vector<T>> jmp;

    min_rmq() = default;
    explicit min_rmq(const vector<T>& V) : jmp(1, V) {
        for (unsigned len = 1, k = 1; len * 2 <= V.size(); len *= 2, ++k) {
            jmp.emplace_back(V.size() - len * 2 + 1);
            for (unsigned j = 0; j < jmp[k].size(); j++) {
                const auto& l = jmp[k - 1][j];
                const auto& r = jmp[k - 1][j + len];
                jmp[k][j] = min(l, r);
            }
        }
    }

    T query(int a, int b) const /* [a,b) */ {
        assert(a < b); // or return inf if a == b
        static constexpr int bits = CHAR_BIT * sizeof(int) - 1;
        int dep = bits - __builtin_clz(b - a);
        const auto& l = jmp[dep][a];
        const auto& r = jmp[dep][b - (1 << dep)];
        return min(l, r);
    }
};

int main() {
    ios::sync_with_stdio(false);
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

    vector<int> depth(N + 1);
    vector<int> parent(N + 1);
    auto index = build_euler_tour_tree_index(tree, root);

    vector<int> reverse_index(2 * N);
    for (int u = 1; u <= N; u++) {
        reverse_index[index[u][0]] = u;
        reverse_index[index[u][1]] = u;
    }

    auto dfs = y_combinator([&](auto self, int u, int p) -> void {
        parent[u] = p;
        depth[u] = depth[p] + 1;
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
            }
        }
    });
    dfs(root, 0);

    // maximum depth
    int D = *max_element(begin(depth) + 1, end(depth));
    int S = sqrt(D);         // bucket size
    int B = (D + S - 1) / S; // number of buckets, including small trailing bucket

    vector<vector<int>> on_depth_index(D + 1);
    vector<vector<int>> on_bucket_index(B);

    auto get_bucket = [S](int d) { return (d - 1) / S; };

    for (int i = 0; i < 2 * N; i++) {
        int u = reverse_index[i];
        int b = get_bucket(depth[u]);
        on_depth_index[depth[u]].push_back(i);
        on_bucket_index[b].push_back(i);
    }

    // Build RMQ for depths
    vector<min_rmq<int>> depth_min(D + 1);
    for (int d = 1; d <= D; d++) {
        const auto& on = on_depth_index[d];
        int M = on.size();
        vector<int> depth_arr(M);
        for (int i = 0; i < M; i++) {
            depth_arr[i] = arr[reverse_index[on[i]]];
        }
        depth_min[d] = min_rmq<int>(depth_arr);
    }

    // Build RMQ for buckets
    vector<min_rmq<int>> bucket_min(B);
    for (int b = 0; b < B; b++) {
        const auto& on = on_bucket_index[b];
        int M = on.size();
        vector<int> bucket_arr(M);
        for (int i = 0; i < M; i++) {
            bucket_arr[i] = arr[reverse_index[on[i]]];
        }
        bucket_min[b] = min_rmq<int>(bucket_arr);
    }

    // Ready to answer queries

    auto query_bucket = [&](int x, int b, int& ans) {
        const auto& on = on_bucket_index[b];
        auto itl = upper_bound(begin(on), end(on), index[x][0]);
        auto itr = lower_bound(itl, end(on), index[x][1]);
        int l = itl - begin(on), r = itr - begin(on);
        if (l < r) {
            ans = min(ans, bucket_min[b].query(l, r));
        }
    };

    auto query_depth = [&](int x, int d, int& ans) {
        const auto& on = on_depth_index[d];
        auto itl = upper_bound(begin(on), end(on), index[x][0]);
        auto itr = lower_bound(itl, end(on), index[x][1]);
        int l = itl - begin(on), r = itr - begin(on);
        if (l < r) {
            ans = min(ans, depth_min[d].query(l, r));
        }
    };

    int Q, last = 0;
    cin >> Q;

    while (Q--) {
        int p, q;
        cin >> p >> q;
        int x = (p + last) % N + 1;
        int k = (q + last) % N;

        int min_depth = min(depth[x] + 1, D);
        int max_depth = min(depth[x] + k, D);

        int min_bucket = get_bucket(min_depth);
        int max_bucket = get_bucket(max_depth);

        last = arr[x];
        if (k > 0 && min_bucket == max_bucket) {
            for (int d = min_depth; d <= max_depth; d++) {
                query_depth(x, d, last);
            }
        } else if (k > 0) {
            for (int b = min_bucket + 1; b < max_bucket; b++) {
                query_bucket(x, b, last);
            }
            for (int d = min_depth; get_bucket(d) == min_bucket; d++) {
                query_depth(x, d, last);
            }
            for (int d = max_depth; get_bucket(d) == max_bucket; d--) {
                query_depth(x, d, last);
            }
        }
        cout << last << endl;
    }

    return 0;
}
