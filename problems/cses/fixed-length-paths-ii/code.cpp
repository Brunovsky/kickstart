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

void merge_reverse_vectors(vector<int>& a, vector<int>& b) {
    if (a.size() < b.size())
        swap(a, b);
    int A = a.size(), B = b.size();
    for (int i = A - 1, j = B - 1; j >= 0; i--, j--)
        a[i] += b[j];
    b.clear();
}

int main() {
    ios::sync_with_stdio(false);
    int N, K1, K2;
    cin >> N >> K1 >> K2;
    vector<vector<int>> tree(N + 1);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    array<long, 2> ans = {};
    array<int, 2> Ks = {K1, K2 + 1};

    vector<int> subsize(N + 1);

    auto dfs_solve = y_combinator([&](auto self, int u, int p) -> vector<int> {
        vector<vector<int>> children;
        subsize[u] = 1;
        children.reserve(tree[u].size());

        for (int v : tree[u]) {
            if (v != p) {
                auto child = self(v, u);
                subsize[u] += subsize[v];
                child.push_back(subsize[v]);
                children.push_back(move(child));
            }
        }

        // sub[s]: how many nodes with depth >=s.
        // depths[d]: how many nodes with depth >=d so far in prefix.
        // we want depth>=K ==> s+d=K
        vector<int> depth = {0, 1};

        for (auto& sub : children) {
            int S = sub.size() - 2, D = depth.size() - 1;
            auto rev_sub = sub.rbegin();
            auto rev_depth = depth.rbegin();
            for (int i : {0, 1}) {
                int s = max(Ks[i] - D, 0), d = Ks[i] - s;
                while (s <= min(S, Ks[i]) && d >= 0) {
                    ans[i] += 1LL * (rev_sub[s] - rev_sub[s + 1]) * rev_depth[d];
                    s++, d--;
                }
                if (s == Ks[i] + 1 && s <= S) {
                    assert(d == -1);
                    ans[i] += 1LL * rev_sub[s] * rev_depth[0];
                }
            }
            merge_reverse_vectors(depth, sub);
        }

        return depth;
    });
    dfs_solve(1, 0);

    cout << ans[0] - ans[1] << endl;
    return 0;
}
