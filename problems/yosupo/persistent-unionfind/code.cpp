#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

struct disjoint_set_rollback {
    int N, S;
    vector<int> next;
    vector<pair<int, int>> history;

    explicit disjoint_set_rollback(int N = 0) : N(N), S(N), next(N, -1) {}

    void assign(int N) { *this = disjoint_set_rollback(N); }
    bool same(int i, int j) { return find(i) == find(j); }
    bool unit(int i) { return next[i] == -1; }
    bool root(int i) { return next[i] < 0; }
    int size(int i) { return -next[i]; }
    int time() const { return history.size(); }

    void rollback() {
        int i = time(), a = i - 1, b = i - 2;
        next[history[a].first] = history[a].second;
        next[history[b].first] = history[b].second;
        history.pop_back(), history.pop_back();
        S++;
    }

    void rollback(int t) {
        int i = time();
        while (i > t) {
            i--, next[history[i].first] = history[i].second;
            i--, next[history[i].first] = history[i].second;
            S++;
        }
        history.resize(t);
    }

    int find(int i) {
        while (next[i] >= 0) {
            i = next[i];
        }
        return i;
    }

    bool join(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (size(i) < size(j)) {
                swap(i, j);
            }
            history.emplace_back(i, next[i]);
            history.emplace_back(j, next[j]);
            next[i] += next[j];
            next[j] = i, S--;
            return true;
        }
        return false;
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

struct linked_lists {
    int L, N;
    vector<int> next, prev;

    // L: lists are [0...L), N: integers are [0...N)
    explicit linked_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    int tail(int l) const { return prev[rep(l)]; }
    bool empty(int l) const { return next[rep(l)] == rep(l); }

    int add_list() { return next.push_back(rep(L)), prev.push_back(rep(L)), L++; }
    void clear(int l) { assert(0 <= l && l < L), next[rep(l)] = prev[rep(l)] = rep(l); }

    void init(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(rep(l), n, rep(l));
    }
    void push_front(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(rep(l), n, head(l));
    }
    void push_back(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(tail(l), n, rep(l));
    }
    void insert_before(int i, int n) {
        assert(0 <= i && i < N && 0 <= n && n < N), meet(prev[i], n, i);
    }
    void insert_after(int i, int n) {
        assert(0 <= i && i < N && 0 <= n && n < N), meet(i, n, next[i]);
    }
    void erase(int n) {
        assert(0 <= n && n < N), meet(prev[n], next[n]); //
    }
    void pop_front(int l) {
        assert(0 <= l && l < L && !empty(l)), meet(rep(l), next[head(l)]);
    }
    void pop_back(int l) {
        assert(0 <= l && l < L && !empty(l)), meet(prev[tail(l)], rep(l));
    }

    void splice_front(int l, int b) {
        assert(0 <= l && l < L && 0 <= b && b < L);
        if (l != b && !empty(b))
            meet(tail(b), head(l)), meet(rep(l), head(b)), clear(b);
    }
    void splice_back(int l, int b) {
        assert(0 <= l && l < L && 0 <= b && b < L);
        if (l != b && !empty(b))
            meet(tail(l), head(b)), meet(tail(b), rep(l)), clear(b);
    }

    void clear() {
        iota(begin(next) + N, end(next), N);
        iota(begin(prev) + N, end(prev), N);
    }
    void assign(int L, int N) {
        this->L = L, this->N = N;
        next.resize(N + L), prev.resize(N + L), clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

#define FOR_EACH_IN_LINKED_LIST(i, l, lists) \
    for (int z##i = l, i = lists.head(z##i); i != lists.rep(z##i); i = lists.next[i])

#define FOR_EACH_IN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int z##i = l, i = lists.tail(z##i); i != lists.rep(z##i); i = lists.prev[i])

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;
    vector<array<int, 2>> nodes(Q);
    linked_lists children(Q + 1, Q), here(Q + 1, Q);
    for (int q = 0; q < Q; q++) {
        int type, k, u, v;
        cin >> type >> k >> u >> v;
        k = k < 0 ? Q : k;
        if (type == 0) {
            children.push_back(k, q);
        } else {
            here.push_back(k, q);
        }
        nodes[q] = {u, v};
    };

    vector<int> answer(Q, -1);
    disjoint_set_rollback dsu(N);

    y_combinator([&](auto self, int p) -> void {
        FOR_EACH_IN_LINKED_LIST (q, p, here) {
            auto [a, b] = nodes[q];
            answer[q] = dsu.same(a, b);
        }
        FOR_EACH_IN_LINKED_LIST (c, p, children) {
            auto [u, v] = nodes[c];
            bool joined = dsu.join(u, v);
            self(c);
            if (joined)
                dsu.rollback();
        }
    })(Q);

    for (int q = 0; q < Q; q++) {
        if (answer[q] != -1) {
            cout << answer[q] << '\n';
        }
    }

    return 0;
}
