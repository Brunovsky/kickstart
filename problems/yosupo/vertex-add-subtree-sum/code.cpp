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

template <typename Fun>
auto y_combinator(Fun&& fun) {
    return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

template <typename T>
struct fenwick {
    int N;
    vector<T> tree;

    explicit fenwick(int N = 0) : N(N), tree(N + 1) {}

    T sum(int i) const {
        T sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= i & -i;
        }
        return sum;
    }

    void add(int i, T n) {
        if (i > 0) {
            while (i <= N) {
                tree[i] += n;
                i += i & -i;
            }
        }
    }

    int lower_bound(T n) const {
        int i = 0;
        int bits = CHAR_BIT * sizeof(int) - __builtin_clz(N << 1);
        for (int j = 1 << bits; j; j >>= 1) {
            if (i + j <= N && tree[i + j] < n) {
                n -= tree[i + j];
                i += j;
            }
        }
        return i + 1;
    }
};

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;

    vector<long> a(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> a[i];
    }

    vector<int> parent(N + 1);
    for (int i = 2; i <= N; i++) {
        cin >> parent[i], parent[i]++;
    }

    vector<vector<int>> tree(N + 1);
    for (int i = 2; i <= N; i++) {
        tree[parent[i]].push_back(i);
    }

    vector<int> tin(N + 1), tout(N + 1);
    int timer = 1;
    y_combinator([&](auto self, int u, int p) -> void {
        tin[u] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                self(v, u);
            }
        }
        tout[u] = timer;
    })(1, 0);

    fenwick<long> fw(N);
    for (int u = 1; u <= N; u++) {
        fw.add(tin[u], a[u]);
    }

    for (int q = 0; q < Q; q++) {
        int type;
        cin >> type;
        if (type == 0) {
            int u, x;
            cin >> u >> x, u++;
            fw.add(tin[u], x);
        } else {
            int u;
            cin >> u, u++;
            long sum = fw.sum(tout[u] - 1) - fw.sum(tin[u] - 1);
            cout << sum << '\n';
        }
    }
    return 0;
}
