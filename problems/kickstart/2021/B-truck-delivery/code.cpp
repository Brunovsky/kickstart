#include <bits/stdc++.h>

using namespace std;

// *****

struct edge_t {
    int v, load;
    long charge;
};

struct query_t {
    int id, weight;
};

struct dyn_segtree {
    int L, R;
    unique_ptr<dyn_segtree> link[2];

    long data = 0;

    void apply(long v) { data = v; }
    auto merge(long a, long b) { return gcd(a, b); }

    explicit dyn_segtree(int L = 0, int R = 0) : L(L), R(R) {}

    void update(int i, long v) {
        assert(L <= i && i < R);
        if (R - L == 1) {
            return apply(v);
        }
        int M = (L + R + 1) / 2;
        if (!link[0]) {
            link[0] = make_unique<dyn_segtree>(L, M);
            link[1] = make_unique<dyn_segtree>(M, R);
        }
        link[i >= M]->update(i, v);
        data = merge(link[0]->data, link[1]->data);
    }

    auto query(int l, int r) {
        if (r <= L || R <= l)
            return 0L;
        if (l <= L && R <= r)
            return data;
        return link[0] ? merge(link[0]->query(l, r), link[1]->query(l, r)) : 0L;
    }
};

constexpr int MAXW = 200'100;
dyn_segtree segtree;
vector<vector<edge_t>> adj;
vector<vector<query_t>> query;
vector<long> query_ans;

void dfs(int u, int p = -1) {
    for (auto [id, weight] : query[u]) {
        query_ans[id] = segtree.query(1, weight + 1);
    }
    for (auto [v, load, charge] : adj[u]) {
        if (v != p) {
            segtree.update(load, charge);
            dfs(v, u);
            segtree.update(load, 0);
        }
    }
}

auto solve() {
    int N, Q;
    cin >> N >> Q;
    adj.assign(N + 1, {});
    query.assign(N + 1, {});
    query_ans.assign(Q, 0);

    for (int i = 0; i < N - 1; i++) {
        int u, v, load;
        long charge;
        cin >> u >> v >> load >> charge;
        adj[u].push_back({v, load, charge});
        adj[v].push_back({u, load, charge});
    }
    for (int q = 0; q < Q; q++) {
        int city, weight;
        cin >> city >> weight;
        query[city].push_back({q, weight});
    }

    segtree = dyn_segtree(0, MAXW);
    dfs(1);

    for (int q = 0; q < Q; q++) {
        cout << ' ' << query_ans[q];
    }
    cout << endl;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
    }
    return 0;
}
