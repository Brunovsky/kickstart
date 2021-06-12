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

auto find_tree_centroids(const vector<vector<int>>& tree) {
    int V = tree.size();
    vector<int> subsize(V), centroids;

    auto dfs_centroid = y_combinator([&](auto self, int u, int p) -> void {
        subsize[u] = 1;
        bool is = true;
        for (int v : tree[u]) {
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
    assert(1u <= centroids.size() && centroids.size() <= 2u);
    return centroids;
}

int main() {
    ios::sync_with_stdio(false);
    int N;
    cin >> N;
    vector<vector<int>> tree(N);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }
    auto centroids = find_tree_centroids(tree);
    cout << centroids[0] + 1 << endl;
    return 0;
}
