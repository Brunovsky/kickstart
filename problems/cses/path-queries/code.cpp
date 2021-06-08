#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

#define max3(a, b, c) max(a, max(b, c))

struct lct_node_path_sum {
    int path_size = 1;
    long self = 0;
    long path = 0;
    long lazy = 0;

    void pushdown(lct_node_path_sum& lhs, lct_node_path_sum& rhs) {
        if (lazy) {
            lhs.lazy += lazy;
            rhs.lazy += lazy;
            self += lazy;
            path += lazy * path_size;
            lazy = 0;
        }
    }

    void pushup(lct_node_path_sum& lhs, lct_node_path_sum& rhs) {
        path_size = 1 + lhs.path_size + rhs.path_size;
        path = self + lhs.path + rhs.path;
    }

    void clear() { lazy = path_size = 0; } // for 0 node
};

template <typename LCTNode>
struct link_cut_tree_path {
    struct Node {
        int parent = 0, child[2] = {};
        int8_t flip = 0; // splay tree is flipped due to reroot
        LCTNode node;
    };

    vector<Node> t;

    explicit link_cut_tree_path(int N = 0) : t(N + 1) { t[0].node.clear(); }

    // ***** Node updates

    // Apply lazy updates stored at u and push them to its children
    void pushdown(int u) {
        auto& [l, r] = t[u].child;
        if (t[u].flip) {
            swap(l, r);
            t[l].flip ^= 1;
            t[r].flip ^= 1;
            t[u].flip = 0;
        }
        t[u].node.pushdown(t[l].node, t[r].node);
    }

    // Update node from splay children and virtual updates
    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(t[l].node, t[r].node);
    }

    // ***** Interface

    void link(int u, int v) {
        reroot(u); // no way to detect cycles without doing extra work
        t[u].parent = v;
    }

    void cut(int u, int v) {
        reroot(u), access(v);
        assert(t[u].parent == v && !t[u].child[1] && u == t[v].child[0]); // no edge!
        t[u].parent = t[v].child[0] = 0;
    }

    void reroot(int u) {
        access(u);
        t[u].flip ^= 1;
    }

    int findroot(int u) {
        access(u);
        while (t[u].child[0])
            u = t[u].child[0], pushdown(u);
        return u;
    }

    int lca(int u, int v) {
        if (u == v)
            return u;
        access(u), v = access(v);
        return t[u].parent ? v : 0;
    }

    LCTNode& query_node(int u) {
        access(u);
        return t[u].node;
    }
    LCTNode& query_path(int u, int v) {
        reroot(u), access(v);
        return t[v].node;
    }
    const LCTNode& query_node(int u) const {
        access(u);
        return t[u].node;
    }
    const LCTNode& query_path(int u, int v) const {
        reroot(u), access(v);
        return t[v].node;
    }

  private:
    bool is_root(int u) const {
        return t[t[u].parent].child[0] != u && t[t[u].parent].child[1] != u;
    }

    void adopt(int parent, int child, int8_t side) {
        if (side >= 0)
            t[parent].child[side] = child;
        if (child)
            t[child].parent = parent;
    }

    void rotate(int u) {
        int p = t[u].parent, g = t[p].parent;
        bool uside = u == t[p].child[1];
        adopt(p, t[u].child[!uside], uside);
        adopt(g, u, !is_root(p) ? p == t[g].child[1] : -1);
        adopt(u, p, !uside);
        pushup(p);
    }

    void splay(int u) {
        int p = t[u].parent, g = t[p].parent;
        while (!is_root(u) && !is_root(p)) {
            pushdown(g), pushdown(p), pushdown(u);
            bool zigzig = (u == t[p].child[1]) == (p == t[g].child[1]);
            rotate(zigzig ? p : u), rotate(u);
            p = t[u].parent, g = t[p].parent;
        }
        if (!is_root(u)) {
            pushdown(p), pushdown(u);
            rotate(u);
        }
        pushdown(u), pushup(u);
    }

    int access(int u) {
        int last = 0, v = u;
        do {
            splay(v);
            t[v].child[1] = last;
            last = v, v = t[v].parent;
        } while (v);
        splay(u);
        assert(!t[u].child[1] && !t[u].flip);
        return last;
    }
};

int main() {
    ios::sync_with_stdio(false);
    int N, Q;
    cin >> N >> Q;
    link_cut_tree_path<lct_node_path_sum> lct(N);
    for (int u = 1, value; u <= N; u++) {
        cin >> value;
        lct.query_node(u).self = value;
    }
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        lct.link(u, v);
    }
    vector<long> ans;
    for (int q = 1; q <= Q; q++) {
        int type;
        cin >> type;
        if (type == 1) {
            int s, x;
            cin >> s >> x;
            lct.query_node(s).self = x;
        } else {
            int s;
            cin >> s;
            ans.push_back(lct.query_path(s, 1).path);
        }
    }
    int A = ans.size();
    for (int i = 0; i < A; i++) {
        cout << ans[i] << "\n";
    }
    return 0;
}
