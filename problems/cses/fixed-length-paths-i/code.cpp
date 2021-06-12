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

auto build_tree_centroid_decomposition(const vector<vector<int>>& tree) {
    int V = tree.size();
    vector<int> subsize(V);
    vector<int> cparent(V, -1);
    vector<bool> mark(V, false);
    int marked = 0;

    auto dfs = y_combinator([&](auto self, int S, int u, int p, int cp) -> bool {
        subsize[u] = 1;
        bool is = true;
        for (int v : tree[u]) {
            if (v != p && !mark[v]) {
                if (self(S, v, u, cp)) {
                    return true;
                } else {
                    subsize[u] += subsize[v];
                    is &= subsize[v] <= S / 2;
                }
            }
        }
        if (is && S - subsize[u] <= S / 2) {
            mark[u] = true, cparent[u] = cp, marked++;
            for (int v : tree[u]) {
                if (v != p && !mark[v]) {
                    self(subsize[v], v, u, u);
                } else if (v == p && !mark[v]) {
                    self(S - subsize[u], v, u, u);
                }
            }
            return true;
        }
        return false;
    });

    dfs(V, 0, -1, -1);
    assert(marked == V);
    return cparent;
}

void merge_deques(deque<int>& a, deque<int>& b) {
    if (a.size() < b.size())
        swap(a, b);
    for (int i = 0, B = b.size(); i < B; i++)
        a[i] += b[i];
}

int main() {
    ios::sync_with_stdio(false);
    int N, K;
    cin >> N >> K;
    vector<vector<int>> tree(N + 1);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    long ans = 0;
    auto dfs_solve = y_combinator([&](auto self, int u, int p) -> deque<int> {
        vector<deque<int>> children;
        for (int v : tree[u]) {
            if (v != p) {
                auto child = self(v, u);
                child.push_front(0);
                children.push_back(move(child));
            }
        }

        deque<int> depths(1, 1);
        for (int c = 0, C = children.size(); c < C; c++) {
            auto& sub = children[c];
            int S = sub.size(), D = depths.size();
            for (int s = max(K - D + 1, 0), d = K - s; s < S && d >= 0; s++, d--) {
                ans += sub[s] * depths[d];
            }
            merge_deques(depths, sub);
        }
        return depths;
    });
    dfs_solve(1, 0);

    cout << ans << endl;
    return 0;
}
