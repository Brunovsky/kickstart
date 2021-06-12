#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

struct Hasher {
    template <typename Container>
    size_t operator()(const Container& vec) const noexcept {
        using std::hash;
        hash<typename Container::value_type> hasher;
        size_t seed = distance(begin(vec), end(vec));
        for (const auto& n : vec) {
            seed ^= hasher(n) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    template <typename U, typename V>
    size_t operator()(const pair<U, V>& p) const noexcept {
        using std::hash;
        size_t a = hash<U>{}(p.first), b = hash<V>{}(p.second);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <typename U>
    size_t operator()(const array<U, 2>& p) const noexcept {
        using std::hash;
        hash<U> hasher;
        size_t a = hasher(p[0]), b = hasher(p[1]);
        return (a + b) * (a + b + 1) / 2 + b;
    }
    template <int i, typename Tuple>
    size_t tuple_compute(const Tuple& tuple) const noexcept {
        if constexpr (i == std::tuple_size_v<Tuple>) {
            return std::tuple_size_v<Tuple>;
        } else {
            using std::hash;
            hash<std::tuple_element_t<i, Tuple>> hasher;
            size_t seed = tuple_compute<i + 1, Tuple>(tuple);
            size_t h = hasher(std::get<i>(tuple));
            return seed ^ (h + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    }
    template <typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const noexcept {
        return tuple_compute<0, tuple<Ts...>>(t);
    }
    size_t operator()(__uint128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return this->operator()(*arr);
    }
    size_t operator()(__int128_t x) const noexcept {
        array<uint64_t, 2>* arr = reinterpret_cast<array<uint64_t, 2>*>(&x);
        return this->operator()(*arr);
    }
};

namespace std {

template <typename T, size_t N>
struct hash<array<T, N>> : Hasher {};
template <typename T>
struct hash<vector<T>> : Hasher {};
template <typename U, typename V>
struct hash<pair<U, V>> : Hasher {};
template <typename... Ts>
struct hash<tuple<Ts...>> : Hasher {};
template <>
struct hash<__int128_t> : Hasher {};
template <>
struct hash<__uint128_t> : Hasher {};

} // namespace std

struct ett_node_sum {
    int subt_size = 0;
    long self = 0;
    long subt = 0;
    long lazy = 0;

    long subtree() const { return subt; }
    int subtree_size() const { return subt_size; }

    void pushdown(bool is_node, bool, ett_node_sum& lhs, ett_node_sum& rhs) {
        if (lazy) {
            lhs.lazy += lazy;
            rhs.lazy += lazy;
            if (is_node)
                self += lazy;
            subt += lazy * subt_size;
            lazy = 0;
        }
    }

    void pushup(bool is_node, bool, const ett_node_sum& lhs, const ett_node_sum& rhs) {
        subt_size = is_node + lhs.subt_size + rhs.subt_size;
        subt = self + lhs.subt + rhs.subt;
    }

    void clear() { subt_size = 0; }
};

/**
 * Unrooted euler tour tree
 */
template <typename ETTNode>
struct euler_tour_tree {
    struct Node {
        int parent = 0, child[2] = {};
        ETTNode node;
    };
    struct access_manager {
      private:
        friend euler_tour_tree;
        euler_tour_tree* ett;
        int u, v, l, r;
        access_manager(euler_tour_tree* ett, int u, int v) : ett(ett), u(u), v(v) {
            int uv = ett->get_edge(u, v), vu = uv ^ 1;
            assert(uv > 0);
            l = ett->splay_split<0>(vu);
            r = ett->splay_split<1>(uv);
            ett->splay(u);
        }
        access_manager(const access_manager&) = delete;
        access_manager(access_manager&&) = delete;

      public:
        ETTNode* operator->() { return &ett->t[u].node; }
        ~access_manager() noexcept { ett->splay_join(l, u, r); }
    };

    int N, F;
    vector<Node> t;
    unordered_map<pair<int, int>, int> edgemap;
    vector<int> freelist;

    explicit euler_tour_tree(int N = 0) : N(N), F(N), t(3 * N + 3), freelist(N) {
        t[0].node.clear();
        edgemap.reserve(2 * N);
        for (int i = 0, f = (N | 1) + 1; i < N; i++) {
            freelist[i] = f + 2 * i;
        }
    }

    int get_edge(int u, int v) const {
        auto ituv = edgemap.find(minmax(u, v));
        return ituv == edgemap.end() ? 0 : u > v ? ituv->second ^ 1 : ituv->second;
    }
    int add_edge(int u, int v) {
        int uv = freelist[--F], vu = uv ^ 1;
        t[uv] = t[vu] = {};
        edgemap.emplace(minmax(u, v), uv).second;
        return u < v ? uv : vu;
    }
    void rem_edge(int u, int v) {
        auto ituv = edgemap.find(minmax(u, v));
        freelist[F++] = ituv->second;
        edgemap.erase(ituv);
    }

  private:
    void pushdown(int u) {
        auto [l, r] = t[u].child;
        if (u != 0) {
            t[u].node.pushdown(u <= N, u > N && !(u & 1), t[l].node, t[r].node);
        }
    }

    void pushup(int u) {
        auto [l, r] = t[u].child;
        pushdown(l), pushdown(r);
        t[u].node.pushup(u <= N, u > N && !(u & 1), t[l].node, t[r].node);
    }

  public:
    bool link(int u, int v) {
        reroot(u), assert(!t[u].parent), reroot(v);
        if (t[u].parent)
            return false;
        int uv = add_edge(u, v), vu = uv ^ 1;
        int r = splay_split<1>(v);
        splay_join(v, splay_join(vu, u, uv), r);
        return true;
    }

    bool cut(int u, int v) {
        int uv = get_edge(u, v), vu = uv ^ 1;
        if (uv == 0)
            return false;

        reroot(v); // v ..A.. vu u ..B.. uv ..C.. --> v ..A.. ..C.. | u ..B..
        splay_join(splice(vu).first, splice(uv).second);
        rem_edge(u, v);
        return true;
    }

    void reroot(int u) { shift_to_front(u); }

    int findroot(int u) {
        splay(u);
        return min_node(u);
    }

    bool conn(int u, int v) {
        splay(u), splay(v);
        return t[u].parent;
    }

    ETTNode* access_node(int u) {
        splay(u);
        return &t[u].node;
    }
    ETTNode* access_edge(int u, int v) {
        int uv = get_edge(min(u, v), max(u, v));
        assert(uv > 0), splay(uv);
        return &t[uv].node;
    }
    ETTNode* access_tree(int u) {
        reroot(u);
        return &t[u].node;
    }
    access_manager access_subtree(int u, int v) {
        reroot(v);
        return access_manager(this, u, v);
    }

  private:
    bool is_root(int u) const { return !t[u].parent; }
    bool is_left(int u) const { return t[u].parent && u == t[t[u].parent].child[0]; }
    bool is_right(int u) const { return t[u].parent && u == t[t[u].parent].child[1]; }
    int root_node(int u) const { return t[u].parent ? root_node(t[u].parent) : u; }
    int min_node(int u) const { return t[u].child[0] ? min_node(t[u].child[0]) : u; }
    int max_node(int u) const { return t[u].child[1] ? max_node(t[u].child[1]) : u; }

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
        adopt(g, u, g ? p == t[g].child[1] : -1);
        adopt(u, p, !uside);
        pushup(p);
    }

    void splay(int u) {
        int p = t[u].parent, g = t[p].parent;
        while (p && g) {
            pushdown(g), pushdown(p), pushdown(u);
            bool zigzig = (u == t[p].child[1]) == (p == t[g].child[1]);
            rotate(zigzig ? p : u), rotate(u);
            p = t[u].parent, g = t[p].parent;
        }
        if (p) {
            pushdown(p), pushdown(u), rotate(u);
        }
        pushdown(u), pushup(u);
    }

    template <bool after>
    int splay_split(int u) {
        splay(u);
        int v = t[u].child[after];
        t[v].parent = t[u].child[after] = 0;
        pushup(u);
        return v;
    }

    int splay_join(int l, int r) {
        if (l == 0 || r == 0)
            return l ? l : r;
        assert(is_root(l) && is_root(r));
        int root = max_node(l);
        splay(root);
        adopt(root, r, 1);
        pushup(root);
        return root;
    }

    int splay_join(int l, int m, int r) { return splay_join(splay_join(l, m), r); }

    auto splice(int u) {
        int r = splay_split<1>(u), l = splay_split<0>(u);
        return make_pair(l, r);
    }

    void shift_to_front(int u) { splay_join(u, splay_split<0>(u)), splay(u); }
    void shift_to_back(int u) { splay_join(splay_split<1>(u), u), splay(u); }
};

int main() {
    ios::sync_with_stdio(false);
    int N, Q;
    cin >> N >> Q;
    euler_tour_tree<ett_node_sum> ett(N);
    for (int u = 1; u <= N; u++) {
        cin >> ett.access_node(u)->self;
    }
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v, u++, v++;
        ett.link(u, v);
    }
    for (int q = 0; q < Q; q++) {
        int type;
        cin >> type;
        if (type == 0) {
            int u, v, w, x;
            cin >> u >> v >> w >> x, u++, v++, w++, x++;
            ett.cut(u, v);
            ett.link(w, x);
        } else if (type == 1) {
            int u, x;
            cin >> u >> x, u++;
            ett.access_node(u)->self += x;
        } else if (type == 2) {
            int v, p;
            cin >> v >> p, v++, p++;
            cout << ett.access_subtree(v, p)->subtree() << endl;
        }
    }
    return 0;
}
