#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <int mod>
struct modnum {
    static_assert(mod > 0 && 2LL * mod < INT_MAX);
    int n;

    modnum() : n(0) {}
    modnum(int v) : n(fit(v % mod)) {}
    explicit operator int() const { return n; }
    explicit operator bool() const { return n != 0; }

    static int fit(int v) { return v >= mod ? v - mod : (v < 0 ? v + mod : v); }
    static int modinv(int v, int m = mod) {
        v %= m, assert(v);
        return v == 1 ? 1 : (m - 1LL * modinv(m, v) * m / v);
    }
    friend modnum modpow(modnum b, long e) {
        modnum p = 1;
        while (e > 0) {
            if (e & 1)
                p = p * b;
            if (e >>= 1)
                b = b * b;
        }
        return p;
    }

    modnum inv() const { return modinv(n); }
    modnum operator-() const { return {fit(-n)}; }
    modnum operator+() const { return {n}; }
    modnum operator++(int) { return n = fit(n + 1), *this - 1; }
    modnum operator--(int) { return n = fit(n - 1), *this + 1; }
    modnum& operator++() { return n = fit(n + 1), *this; }
    modnum& operator--() { return n = fit(n - 1), *this; }
    modnum& operator+=(modnum v) { return n = fit(n + v.n), *this; }
    modnum& operator-=(modnum v) { return n = fit(n - v.n), *this; }
    modnum& operator*=(modnum v) { return n = (1LL * n * v.n) % mod, *this; }
    modnum& operator/=(modnum v) { return n = (1LL * n * modinv(v.n)) % mod, *this; }

    friend modnum operator+(modnum lhs, modnum rhs) { return lhs += rhs; }
    friend modnum operator-(modnum lhs, modnum rhs) { return lhs -= rhs; }
    friend modnum operator*(modnum lhs, modnum rhs) { return lhs *= rhs; }
    friend modnum operator/(modnum lhs, modnum rhs) { return lhs /= rhs; }

    friend string to_string(modnum v) { return to_string(v.n); }
    friend bool operator==(modnum lhs, modnum rhs) { return lhs.n == rhs.n; }
    friend bool operator!=(modnum lhs, modnum rhs) { return lhs.n != rhs.n; }
    friend ostream& operator<<(ostream& out, modnum v) { return out << v.n; }
    friend istream& operator>>(istream& in, modnum& v) {
        return in >> v.n, v.n = fit(v.n % mod), in;
    }
};

using num = modnum<998'244'353>;

struct lct_node_path_composite {
    using Data = array<num, 2>;
    Data self = {};
    Data path[2] = {};

    void read() { cin >> self[0] >> self[1]; }
    num eval(num x) const { return path[1][0] * x + path[1][1]; }
    Data combine(Data a, Data b) { return Data{a[0] * b[0], a[0] * b[1] + a[1]}; }

    void path_flip() { swap(path[0], path[1]); }

    void pushdown(lct_node_path_composite&, lct_node_path_composite&) {}

    void pushup(const lct_node_path_composite& lhs, const lct_node_path_composite& rhs) {
        path[0] = combine(combine(lhs.path[0], self), rhs.path[0]); // lhs(self(rhs(x)))
        path[1] = combine(combine(rhs.path[1], self), lhs.path[1]); // rhs(self(lhs(x)))
    }

    void clear() { self[0] = path[0][0] = path[1][0] = 1; } // for 0 node
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
  private:
    void pushdown(int u) {
        auto& [l, r] = t[u].child;
        if (t[u].flip) {
            swap(l, r);
            t[l].flip ^= 1;
            t[r].flip ^= 1;
            t[u].flip = 0;
            t[u].node.path_flip();
        }
        if (u != 0) {
            t[u].node.pushdown(t[l].node, t[r].node);
        }
    }

    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(t[l].node, t[r].node);
    }

    // ***** Interface
  public:
    bool link(int u, int v) {
        reroot(u), access(v); // no way to detect cycles without doing extra work
        if (t[u].parent)
            return false;
        t[u].parent = v;
        return true;
    }

    bool cut(int u, int v) {
        reroot(u), access(v);
        if (!t[u].parent || t[u].child[1] || u != t[v].child[0])
            return false;
        t[u].parent = t[v].child[0] = 0;
        return true;
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

    bool conn(int u, int v) { return lca(u, v) != 0; }

    LCTNode* access_node(int u) {
        access(u);
        return &t[u].node;
    }
    LCTNode* access_path(int u, int v) {
        reroot(u), access(v);
        return &t[v].node;
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
    link_cut_tree_path<lct_node_path_composite> lct(N);
    for (int u = 1; u <= N; u++) {
        lct.access_node(u)->read();
    }
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v, u++, v++;
        lct.link(u, v);
    }
    for (int q = 0; q < Q; q++) {
        int type;
        cin >> type;
        if (type == 0) {
            int u, v, w, x;
            cin >> u >> v >> w >> x, u++, v++, w++, x++;
            lct.cut(u, v);
            lct.link(w, x);
        } else if (type == 1) {
            int u;
            cin >> u, u++;
            lct.access_node(u)->read();
        } else {
            int u, v;
            num x;
            cin >> u >> v >> x, u++, v++;
            cout << lct.access_path(u, v)->eval(x) << endl;
        }
    }
    return 0;
}
