#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

template <typename Node, typename Update>
struct segtree {
    vector<Node> node;
    vector<Update> update;
    vector<bool> has_lazy;
    vector<array<int, 2>> range;

    segtree() = default;
    segtree(int L, int R) { assign(L, R); }

    template <typename Arr>
    segtree(int L, int R, const Arr& arr) {
        assign(L, R, arr);
    }

    template <typename Arr>
    void assign(int L, int R, const Arr& arr) {
        int N = R - L;
        node.assign(2 * N, Node());
        update.assign(2 * N, Update());
        has_lazy.assign(2 * N, false);
        range.resize(2 * N);
        int Q = 1 << (N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0);
        for (int i = 0; i < N; i++) {
            range[i + N] = {L + i, L + i + 1};
            node[i + N] = Node(arr[L + i]);
        }
        rotate(begin(node) + N, begin(node) + (3 * N - Q), end(node));
        rotate(begin(range) + N, begin(range) + (3 * N - Q), end(range));
        for (int u = N - 1; u >= 1; u--) {
            int cl = u << 1, cr = u << 1 | 1;
            range[u] = {range[cl][0], range[cr][1]};
            node[u].merge(node[cl], node[cr]);
        }
    }

    void assign(int L, int R) {
        int N = R - L;
        node.assign(2 * N, Node());
        update.assign(2 * N, Update());
        has_lazy.assign(2 * N, false);
        range.resize(2 * N);
        int Q = 1 << (N > 1 ? 8 * sizeof(N) - __builtin_clz(N - 1) : 0);
        for (int i = 0; i < N; i++) {
            range[i + N] = {L + i, L + i + 1};
        }
        rotate(begin(range) + N, begin(range) + (3 * N - Q), end(range));
        for (int u = N - 1; u >= 1; u--) {
            int cl = u << 1, cr = u << 1 | 1;
            range[u] = {range[cl][0], range[cr][1]};
        }
    }

    auto query_range(int L, int R) { return query_range(1, L, R); }
    void update_range(int L, int R, const Update& add) { update_range(1, L, R, add); }

  private:
    void pushdown(int u) {
        if (has_lazy[u]) {
            int cl = u << 1, cr = u << 1 | 1;
            has_lazy[cl] = has_lazy[cr] = 1;
            update[u].apply(node[cl], range[cl]);
            update[u].apply(node[cr], range[cr]);
            update[cl].merge(update[u], range[cl]);
            update[cr].merge(update[u], range[cr]);
            has_lazy[u] = 0;
            update[u] = Update();
        }
    }

    void update_range(int u, int L, int R, const Update& add) {
        if (R <= range[u][0] || range[u][1] <= L) {
            return;
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            add.apply(node[u], range[u]);
            update[u].merge(add, range[u]);
            has_lazy[u] = 1;
            return;
        }
        pushdown(u);
        int cl = u << 1, cr = u << 1 | 1;
        update_range(cl, L, R, add);
        update_range(cr, L, R, add);
        node[u].merge(node[cl], node[cr]);
    }

    auto query_range(int u, int L, int R) {
        if (R <= range[u][0] || range[u][1] <= L) {
            return Node();
        }
        if (L <= range[u][0] && range[u][1] <= R) {
            return node[u];
        }
        pushdown(u);
        int cl = u << 1, cr = u << 1 | 1;
        Node ans;
        ans.merge(query_range(cl, L, R), query_range(cr, L, R));
        return ans;
    }
};

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

    modnum inv() const { return {modinv(n)}; }
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

using num = modnum<998244353>;

struct affine_segnode {
    num value = 0;
    explicit affine_segnode(num value = 0) : value(value) {}
    void merge(const affine_segnode& lhs, const affine_segnode& rhs) {
        value = lhs.value + rhs.value;
    }
};

struct add_segupdate {
    num b = 1, c = 0;
    add_segupdate() : b(1), c(0) {}
    add_segupdate(num b, num c) : b(b), c(c) {}
    void merge(const add_segupdate& parent, array<int, 2> /*range*/) {
        c = c * parent.b + parent.c;
        b = b * parent.b;
    }
    void apply(affine_segnode& node, array<int, 2> range) const {
        node.value = b * node.value + c * (range[1] - range[0]);
    }
};

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;
    vector<num> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    segtree<affine_segnode, add_segupdate> tree(0, N, A);
    for (int q = 0; q < Q; q++) {
        int type;
        cin >> type;
        if (type == 0) {
            int l, r;
            num b, c;
            cin >> l >> r >> b >> c;
            tree.update_range(l, r, add_segupdate(b, c));
        } else {
            int l, r;
            cin >> l >> r;
            cout << tree.query_range(l, r).value << endl;
        }
    }
    return 0;
}
