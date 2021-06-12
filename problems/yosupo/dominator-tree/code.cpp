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

auto build_dominator_tree(int root, const vector<vector<int>>& out) {
    int V = out.size();
    assert(V > 0 && root > 0 && root < V);

    vector<vector<int>> in(V);
    vector<int> dom(V, 0);
    vector<int> parent(V, 0);
    vector<int> semi(V, 0);
    vector<int> vertex(V, 0);
    vector<int> ancestor(V, 0);
    vector<int> label(V, 0);
    iota(begin(label), end(label), 0);
    int timer = 1;

    y_combinator([&](auto self, int u) -> void {
        vertex[timer] = u, semi[u] = timer++;
        for (int v : out[u]) {
            if (semi[v] == 0) {
                parent[v] = u;
                self(v);
            }
        }
    })(root);

    for (int u = 1; u < V; u++) {
        if (semi[u] > 0) { // skip unreachable nodes
            for (int v : out[u]) {
                in[v].push_back(u);
            }
        }
    }

    auto compress = y_combinator([&](auto self, int v) -> void {
        if (ancestor[ancestor[v]] != 0) {
            self(ancestor[v]);
            if (semi[label[v]] > semi[label[ancestor[v]]]) {
                label[v] = label[ancestor[v]];
            }
            ancestor[v] = ancestor[ancestor[v]];
        }
    });

    auto eval = [&](int v) -> int {
        if (ancestor[v] == 0) {
            return v;
        } else {
            compress(v);
            return label[v];
        }
    };

    vector<int> bucket_head(V, 0), bucket_next(V, 0);

    for (int i = V - 1; i >= 2; i--) {
        int w = vertex[i];
        for (int v : in[w]) {
            int u = eval(v);
            semi[w] = min(semi[w], semi[u]);
        }
        // push w onto the front of bucket b
        int b = vertex[semi[w]];
        bucket_next[w] = bucket_head[b], bucket_head[b] = w;
        ancestor[w] = parent[w]; // link
        // visit all nodes in bucket parent[w]
        for (int v = bucket_head[parent[w]]; v != 0; v = bucket_next[v]) {
            int u = eval(v);
            dom[v] = semi[u] < semi[v] ? u : parent[w];
        }
        bucket_head[parent[w]] = 0;
    }

    for (int i = 2; i < V; i++) {
        int w = vertex[i];
        if (dom[w] != vertex[semi[w]]) {
            dom[w] = dom[dom[w]];
        }
    }
    dom[root] = 0;

    // might wish to return semi as well
    return make_pair(dom, parent);
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, M, S;
    cin >> N >> M >> S, S++;
    vector<vector<int>> out(N + 1);
    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v, u++, v++;
        out[u].push_back(v);
    }
    auto [dom, parent] = build_dominator_tree(S, out);
    dom[S] = parent[S] = S;
    for (int u = 1; u <= N; u++) {
        cout << dom[u] - 1 << " \n"[u == N];
    }
    return 0;
}
