#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

struct lca_rmq {
    int N, B = 0, M = 0;
    vector<int> tour, first, last, up, depth;
    vector<vector<int>> jmp;

    static int need_bits(int n) {
        return 1 + (n > 1 ? 8 * sizeof(n) - __builtin_clz(n - 1) : 0);
    }

    explicit lca_rmq(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), first(N), last(N), up(N), depth(N) {
        init_dfs(tree, root, zero);

        jmp = {tour};

        for (int len = 1, k = 1; 2 * len <= M; len *= 2, k++) {
            int J = M - 2 * len + 1;
            jmp.emplace_back(J);
            for (int j = 0; j < J; j++) {
                int l = jmp[k - 1][j];
                int r = jmp[k - 1][j + len];
                jmp[k][j] = min(make_pair(depth[l], l), make_pair(depth[r], r)).second;
            }
        }
    }

    void init_dfs(const vector<vector<int>>& tree, int u, int p) {
        up[u] = p;
        first[u] = last[u] = M++;
        tour.push_back(u);
        for (int v : tree[u]) {
            if (v != p) {
                depth[v] = depth[u] + 1;
                init_dfs(tree, v, u);
                last[u] = M++;
                tour.push_back(u);
            }
        }
    }

    int parent(int u) const { return up[u]; }

    int lca(int u, int v) const {
        static constexpr int BITS = CHAR_BIT * sizeof(int) - 1;
        if (u == v)
            return u;
        auto [a, b] = minmax(first[u], first[v]);
        int bits = BITS - __builtin_clz(b - a);
        int l = jmp[bits][a];
        int r = jmp[bits][b - (1 << bits)];
        return min(make_pair(depth[l], l), make_pair(depth[r], r)).second;
    }
};

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

struct lca_schieber_vishkin {
    int N, timer = 0;
    vector<int> preorder, up, I, A, head;

    static int lowest_one_bit(int n) { return n & -n; }
    static int highest_one_bit(int n) { return n ? 1 << (31 - __builtin_clz(n)) : 0; }

    explicit lca_schieber_vishkin(const vector<vector<int>>& tree, int root, int zero = 0)
        : N(tree.size()), preorder(N), up(N), I(N), A(N), head(N) {
        init_dfs1(tree, root, zero);
        init_dfs2(tree, root, zero, 0);
    }

    void init_dfs1(const vector<vector<int>>& tree, int u, int p) {
        up[u] = p;
        I[u] = preorder[u] = timer++;
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs1(tree, v, u);
                if (lowest_one_bit(I[u]) < lowest_one_bit(I[v])) {
                    I[u] = I[v];
                }
            }
        }
        head[I[u]] = u;
    }

    void init_dfs2(const vector<vector<int>>& tree, int u, int p, int up) {
        A[u] = up | lowest_one_bit(I[u]);
        for (int v : tree[u]) {
            if (v != p) {
                init_dfs2(tree, v, u, A[u]);
            }
        }
    }

    int parent(int u) const { return up[u]; }

    int enter_into_strip(int x, int hz) const {
        if (lowest_one_bit(I[x]) == hz)
            return x;
        int hw = highest_one_bit(A[x] & (hz - 1));
        return parent(head[(I[x] & -hw) | hw]);
    }

    int lca(int x, int y) const {
        int hb = I[x] == I[y] ? lowest_one_bit(I[x]) : highest_one_bit(I[x] ^ I[y]);
        int hz = lowest_one_bit(A[x] & A[y] & -hb);
        int ex = enter_into_strip(x, hz);
        int ey = enter_into_strip(y, hz);
        return preorder[ex] < preorder[ey] ? ex : ey;
    }
};

int main() {
    ios::sync_with_stdio(false);
    int N, M;
    cin >> N >> M;
    vector<vector<int>> tree(N + 1);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }
    lca_schieber_vishkin lca(tree, 1);
    vector<int> delta(N + 1);
    for (int m = 0; m < M; m++) {
        int a, b;
        cin >> a >> b;
        int c = lca.lca(a, b);
        if (c == a) {
            delta[b]++, delta[lca.parent(c)]--;
        } else if (c == b) {
            delta[a]++, delta[lca.parent(c)]--;
        } else {
            delta[a]++, delta[b]++, delta[c]--, delta[lca.parent(c)]--;
        }
    }
    vector<int> ans(N + 1);
    auto dfs = y_combinator([&](auto self, int u, int p) -> int {
        int sum = delta[u];
        for (int v : tree[u]) {
            if (v != p) {
                sum += self(v, u);
            }
        }
        ans[u] = sum;
        return sum;
    });
    dfs(1, 0);
    for (int u = 1; u <= N; u++) {
        cout << ans[u] << " \n"[u == N];
    }
    return 0;
}
