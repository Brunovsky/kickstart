#ifndef INTEGER_DATA_STRUCTURES_HPP
#define INTEGER_DATA_STRUCTURES_HPP

#include "bits.hpp"

// *****

/**
 * 2d dancing links, non-circular
 */
struct dancing_links_matrix {
    int n = 0, m = 0;
    vector<vector<int>> S, N, E, W;

    dancing_links_matrix() = default;
    dancing_links_matrix(int n, int m) { init(n, m); }

    void init(int n, int m) {
        this->n = n, this->m = m;
        N.assign(n + 2, vector<int>(m + 2));
        S.assign(n + 2, vector<int>(m + 2));
        E.assign(n + 2, vector<int>(m + 2));
        W.assign(n + 2, vector<int>(m + 2));
        for (int i = 0; i <= n + 1; i++) {
            for (int j = 0; j <= m + 1; j++) {
                S[i][j] = i + 1;
                N[i][j] = i - 1;
                E[i][j] = j + 1;
                W[i][j] = j - 1;
            }
        }
    }
    using int2 = array<int, 2>;
    void insert(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = c;
        W[r][E[r][c]] = c;
        S[N[r][c]][c] = r;
        N[S[r][c]][c] = r;
    }
    void remove(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = E[r][c];
        W[r][E[r][c]] = W[r][c];
        S[N[r][c]][c] = S[r][c];
        N[S[r][c]][c] = N[r][c];
    }
    int2 south(int2 p) const {
        int r = p[0], c = p[1];
        return {S[r][c], c};
    }
    int2 north(int2 p) const {
        int r = p[0], c = p[1];
        return {N[r][c], c};
    }
    int2 east(int2 p) const {
        int r = p[0], c = p[1];
        return {r, E[r][c]};
    }
    int2 west(int2 p) const {
        int r = p[0], c = p[1];
        return {r, W[r][c]};
    }
};

/**
 * Optimized versions of simple data structures like trees, queues and lists for special
 * case over the integers [0...M).
 * Parent/child/list relationships are kept in vectors instead of pointers.
 *
 * Ideas from: https://github.com/indy256/codelibrary
 */

/**
 * Dynamic queue over the integers [0...M), allowing ints to be released from anywhere
 * in the queue.
 */
struct int_freelist {
    int N, head = 0;
    vector<int> next;

    // N: number of integers (integers[0...N))
    explicit int_freelist(int N = 0) { assign(N); }

    bool empty() const { return head == N; }
    bool taken(int n) { return next[n] != -1; }
    int request(int n = 0) { return n = head, head = next[n], next[n] = -1, n; }
    void release(int n) { assert(next[n] == -1), next[n] = head, head = n; }
    void clear() { head = N, fill(begin(next), end(next), -1); }
    void reset() { head = 0, iota(begin(next), end(next), 1); }
    void assign(int N) { this->N = N, next.resize(N), reset(); }
};

/**
 * Numbered collection of forward linked lists over the integers [0...N).
 * Each list maintains a head pointer and each int maintains a next pointer.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next pointers are not reset by clear()
 * Usage examples:
 *     forward adjacency lists for directed graphs
 *         fl.push(u, e) // add edge e to node u's adjacency list
 *         FOR_EACH_IN_FORWARD_LIST(e, u, fl) // iterate u's outedges
 *     directed trees with only parent pointers
 *         fl.push(p, c) // set the parent of c to p
 *     bucket sorting
 *         fl.push(b, n) // add value n to bucket b
 */
struct forward_lists {
    int L, N;
    vector<int> next;

    // L: lists are [0...L), N: integers are [0...N)
    explicit forward_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    bool empty(int l) const { return head(l) == -1; }

    void init(int l, int n) { next[rep(l)] = n, next[n] = -1; }
    void clear(int l) { next[rep(l)] = -1; }
    void exchange(int l, int t) { swap(next[rep(l)], next[rep(t)]); }

    void push(int l, int n) { insert(rep(l), n); }
    void insert(int i, int n) { next[n] = next[i], next[i] = n; }
    void pop(int l) { assert(head(l) != -1), next[rep(l)] = next[head(l)]; }

    void clear() { fill(begin(next) + N, end(next), -1); }
    void assign(int L, int N) { this->L = L, this->N = N, next.assign(L + N, -1); }
};

#define FOR_EACH_IN_FORWARD_LIST(i, l, lists) \
    for (int i = lists.head(l); i != -1; i = lists.next[i])

/**
 * Numbered collection of doubly linked lists over the integers [0...N)
 * Each list maintains head/tail pointers and each int maintains next/prev pointers.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next/prev pointers (for integers) are not reset by clear()
 */
struct linked_lists {
    int L, N;
    vector<int> next, prev;

    // L: lists are [0...L), N: integers are [0...N)
    explicit linked_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    int tail(int l) const { return prev[rep(l)]; }
    bool empty(int l) const { return next[rep(l)] == rep(l); }

    void init(int l, int n) { meet(rep(l), n, rep(l)); }
    void clear(int l) { next[rep(l)] = prev[rep(l)] = rep(l); }
    void exchange(int l, int t) {
        swap(next[rep(l)], next[rep(t)]), swap(prev[rep(l)], prev[rep(t)]);
    }

    void push_front(int l, int n) { meet(rep(l), n, head(l)); }
    void push_back(int l, int n) { meet(tail(l), n, rep(l)); }
    void insert_before(int i, int n) { meet(prev[i], n, i); }
    void insert_after(int i, int n) { meet(i, n, next[i]); }
    void erase(int n) { meet(prev[n], next[n]); }
    void pop_front(int l) { meet(rep(l), next[head(l)]); }
    void pop_back(int l) { meet(prev[tail(l)], rep(l)); }

    void splice_front(int l, int b) { assert(l != b), splice_before(head(l), b); }
    void splice_back(int l, int b) { assert(l != b), splice_after(tail(l), b); }
    void splice_before(int i, int b) {
        if (!empty(b))
            meet(prev[i], head(b)), meet(tail(b), i), clear(b);
    }
    void splice_after(int i, int b) {
        if (!empty(b))
            meet(tail(b), next[i]), meet(i, head(b)), clear(b);
    }

    void clear() {
        iota(begin(next) + N, end(next), N), iota(begin(prev) + N, end(prev), N);
    }
    void assign(int L, int N) {
        this->L = L, this->N = N, next.resize(N + L), prev.resize(N + L), clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

#define FOR_EACH_IN_LINKED_LIST(i, l, lists) \
    for (int i = lists.head(l); i != lists.rep(l); i = lists.next[i])

#define FOR_EACH_IN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int i = lists.tail(l); i != lists.rep(l); i = lists.prev[i])

/**
 * Forest of rooted trees over the integers [0...N) with basic splice operations
 * for forward lists.
 * Each node maintains a parent pointer and a next pointer for its siblings. Each
 * node may or may not belong to the forest. The forest is implicitly rooted on the
 * phantom node N. Roots of the forest are the childs of N. Each node n has a
 * representative rep(n) which is the first node of its child list.
 * With appropriate usage the trees are kept acyclic and disjoint, but the usage can be
 * more exotic.
 */
struct int_forest {
    int N, S = 0;
    vector<int> parent, next;

    // N: integers are [0...N)
    explicit int_forest(int N = 0) { assign(N); }

    bool is_root(int n) const { return n < N && parent[n] == N; }
    int rep(int n) const { return n + N + 1; }
    int head(int n) const { return next[rep(n)]; }
    bool contains(int n) const { return parent[n] != -1; }
    bool empty(int n) const { return head(n) == -1; }
    bool empty() const { return S == 0; }

    void init(int n) { splice_front(N, n); }
    void clear(int n) {
        assert(0 <= n && n < N && contains(n) && n == head(parent[n]));
        S--, next[rep(parent[n])] = next[n], parent[n] = -1;
    }
    void splice_front(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && p != n && parent[n] != p);
        assert((!contains(n) || n != head(parent[n])) && contains(p));
        S += !contains(n), next[n] = next[rep(p)], next[rep(p)] = n, parent[n] = p;
    }
    void splice_after(int i, int n) {
        assert(0 <= i && i < N && 0 <= n && n < N && i != n);
        assert((!contains(n) || n != head(parent[n])) && contains(i));
        S += !contains(n), next[n] = next[i], next[i] = n, parent[n] = parent[i];
    }
    void splice_children_front(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && p != n && contains(p));
        int v, u = head(n);
        while (u != -1)
            v = next[u], splice_front(p, u), u = v;
        next[rep(n)] = -1;
    }
    void splice_children_after(int i, int n) {
        assert(0 <= n && n < N && 0 <= i && i < N && i != n && contains(i));
        int v, u = head(n);
        while (u != -1)
            v = next[u], splice_after(i, u), u = v;
        next[rep(n)] = -1;
    }
    void sibling_adopt_self(int n) {
        assert(0 <= n && n < N && next[n] != -1 && contains(n));
        int m = next[n], u = next[m];    // n->m->u   becomes   n->u
        splice_front(n, m), next[n] = u; //                     m
    }
    void sibling_adopt_next(int n) {
        assert(0 <= n && n != N && next[n] != -1 && next[next[n]] != -1);
        int m = next[n], u = next[m];    // n->m->u   becomes   n->u
        splice_front(u, m), next[n] = u; //                        m
    }

    void clear() {
        int R = N + 1, E = 2 * R;
        auto x = parent.data(), n = next.data();
        S = 0;
        fill(x, x + N, -1); // parent[n] = -1
        fill(n, n + E, -1); // next[n] = head(n) = -1
    }
    void assign(int N) {
        this->N = N;
        parent.resize(N), next.resize(2 * N + 2);
        clear();
    }
};

/**
 * Forest of rooted trees over the integers [0...N) with basic splice operations
 * for doubly linked lists.
 * Each node maintains a parent pointer and a next/prev pointer for its siblings. Each
 * node may or may not belong to the forest. The forest is implicitly rooted on the
 * phantom node N. Roots of the forest are the childs of N. Each node n has a
 * representative rep(n) which is the first node of its child list.
 * With appropriate usage the trees are kept acyclic and disjoint, but the usage can be
 * more exotic.
 */
struct linked_int_forest {
    int N, S = 0;
    vector<int> parent, next, prev;

    // N: integers are [0...N)
    explicit linked_int_forest(int N = 0) { assign(N); }

    bool is_rep(int n) const { return n > N; }
    bool is_root(int n) const { return parent[n] == N; }
    int rep(int n) const { return n + N + 1; }
    int head(int n) const { return next[rep(n)]; }
    int tail(int n) const { return prev[rep(n)]; }
    bool contains(int n) const { return parent[n] != -1; }
    bool empty(int n) const { return head(n) > N; }
    bool empty() const { return S == 0; }

    void init(int n) { splice_back(N, n); }
    void clear(int n) {
        assert(0 <= n && n < N && contains(n) && empty(n));
        S--, parent[n] = -1;
        meet(prev[n], next[n]), prev[n] = next[n] = rep(n);
    }
    void splice_front(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && n != p && (p == N || contains(p)));
        S += !contains(n), parent[n] = p;
        meet(prev[n], next[n]), meet(rep(p), n, head(p));
    }
    void splice_back(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && n != p && (p == N || contains(p)));
        S += !contains(n), parent[n] = p;
        meet(prev[n], next[n]), meet(tail(p), n, rep(p));
    }
    void splice_after(int s, int n) {
        assert(0 <= n && n < N && 0 <= s && s <= N && s != n && contains(s));
        S += !contains(n), parent[n] = parent[s];
        meet(prev[n], next[n]), meet(s, n, next[s]);
    }
    void splice_before(int s, int n) {
        assert(0 <= n && n < N && 0 <= s && s <= N && s != n && contains(s));
        S += !contains(n), parent[n] = parent[s];
        meet(prev[n], next[n]), meet(prev[s], n, s);
    }
    void splice_children_front(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && n != p && (p == N || contains(p)));
        if (!empty(n)) {
            reparent(head(n), rep(n), p);
            meet(rep(p), head(n)), meet(tail(n), head(p));
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }
    void splice_children_back(int p, int n) {
        assert(0 <= n && n < N && 0 <= p && p <= N && n != p && (p == N || contains(p)));
        if (!empty(n)) {
            reparent(head(n), rep(n), p);
            meet(tail(p), head(n)), meet(tail(n), rep(p));
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }
    void splice_children_after(int s, int n) {
        assert(0 <= n && n < N && 0 <= s && s < N && n != s && contains(s));
        if (!empty(n)) {
            reparent(head(n), rep(n), parent[s]);
            meet(tail(n), next[s]), meet(s, head(n));
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }
    void splice_children_before(int s, int n) {
        assert(0 <= n && n < N && 0 <= s && s < N && n != s && contains(s));
        if (!empty(n)) {
            reparent(head(n), rep(n), parent[s]);
            meet(prev[s], head(n)), meet(tail(n), s);
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }
    void splice_children_front_unsafe(int n) {
        assert(0 <= n && n < N);
        if (!empty(n)) {
            meet(rep(N), head(n)), meet(tail(n), head(N));
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }
    void splice_children_back_unsafe(int n) {
        assert(0 <= n && n < N);
        if (!empty(n)) {
            meet(tail(N), head(n)), meet(tail(n), rep(N));
            next[rep(n)] = prev[rep(n)] = rep(n);
        }
    }

    void clear() {
        int R = N + 1, E = 2 * R;
        auto x = parent.data(), n = next.data(), p = prev.data();
        S = 0;
        fill(x + 0, x + N, -1); // parent[n] = -1
        iota(n + 0, n + R, R);  // next[n] = rep(n)
        iota(n + R, n + E, R);  // next[rep(n)] = rep(n)
        iota(p + 0, p + R, R);  // prev[n] = rep(n)
        iota(p + R, p + E, R);  // prev[rep(n)] = rep(n)
    }
    void assign(int N) {
        this->N = N;
        parent.resize(N), next.resize(2 * N + 2), prev.resize(2 * N + 2);
        clear();
    }

  private:
    inline void meet(int n, int m) { next[n] = m, prev[m] = n; }
    inline void meet(int n, int m, int k) { meet(n, m), meet(m, k); }
    inline void reparent(int u, int e, int p) {
        while (u != e)
            parent[u] = p, u = next[u];
    }
};

#define FOR_EACH_CHILD_INT_TREE(i, n, forest) \
    for (int i = forest.head(n); !forest.is_rep(i); i = forest.next[i])

#define FOR_EACH_CHILD_INT_TREE_REVERSE(i, n, forest) \
    for (int i = forest.tail(n); !forest.is_rep(i); i = forest.prev[i])

#define FOR_EACH_NEXT_SIBLING_INT_TREE(i, s, forest) \
    for (int i = forest.next[s]; !forest.is_rep(i); i = forest.next[i])

#define FOR_EACH_PREV_SIBLING_INT_TREE(i, s, forest) \
    for (int i = forest.prev[s]; !forest.is_rep(i); i = forest.prev[i])

#define FOR_EACH_ANCESTOR_INT_TREE(i, n, forest) \
    for (int i = forest.parent[n]; i != forest.N; i = forest.parent[i])

/**
 * Binary heap over the integers [0...N) with decrease-key.
 * By default a min-heap, but you'll usually need a custom compare (e.g. dijkstra).
 *
 *           O(log n)     push(u)
 *           O(log n)-    decrease_key(u)
 *           O(log n)-    increase_key(u)
 *             O(1)       top()
 *           O(log n)*    pop()
 */
template <typename Compare = less<>>
struct binary_int_heap {
    vector<int> c, id;
    Compare comp;

    explicit binary_int_heap(int N = 0, const Compare& comp = Compare())
        : c(0, 0), id(N, -1), comp(comp) {}

    bool empty() const { return c.empty(); }
    size_t size() const { return c.size(); }
    bool contains(int n) const { return id[n] != -1; }
    int top() const { return c[0]; }

    void push(int n) {
        assert(!contains(n));
        id[n] = c.size(), c.push_back(n);
        heapify_up(id[n]);
    }

    int pop() {
        assert(!empty());
        int n = c[0];
        swap(c[0], c.back());
        id[c[0]] = 0, id[n] = -1;
        c.pop_back();
        heapify_down(0);
        return n;
    }

    void improve(int n) { assert(contains(n)), heapify_up(id[n]); }
    void decline(int n) { assert(contains(n)), heapify_down(id[n]); }
    void push_or_improve(int n) { contains(n) ? improve(n) : push(n); }
    void push_or_decline(int n) { contains(n) ? decline(n) : push(n); }

  private:
    inline int parent(int i) const { return (i - 1) >> 1; }
    inline int child(int i) const { return i << 1 | 1; }
    void heapify_up(int i) {
        while (i > 0 && comp(c[i], c[parent(i)])) { // while c[i] < c[parent(i)]
            exchange(i, parent(i)), i = parent(i);
        }
    }
    void heapify_down(int i) {
        int k, S = c.size();
        while ((k = child(i)) < S) {
            if (k + 1 < S && !comp(c[k], c[k + 1])) // if c[rchild(i)] <= c[lchild(i)]
                k++;
            if (!comp(c[k], c[i])) // break if c[i] <= c[minchild(i)]
                break;
            exchange(i, k), i = k;
        }
    }
    void exchange(int i, int j) { swap(id[c[i]], id[c[j]]), swap(c[i], c[j]); }
};

/**
 * Pairing heap over the unique integers [0...N)
 * By default a min-heap, but you'll usually need a custom compare.
 * Unfortunately slower than binary_int_heap in practice.
 *
 * Operation complexities
 *             O(1)       push(u)
 *         O(log log n)*  decrease_key(u)
 *             O(1)       top()
 *           O(log n)*    pop()
 */
template <typename Compare = less<>>
struct pairing_int_heap {
    linked_int_forest forest;
    Compare comp;

    explicit pairing_int_heap(int N, const Compare& comp = Compare())
        : forest(N), comp(comp) {}

    bool empty() const { return forest.empty(); }
    size_t size() const { return forest.S; }
    bool contains(int n) const { return forest.contains(n); }
    int top() const { return forest.head(forest.N); }

    void push(int n) {
        assert(0 <= n && n < forest.N && !contains(n));
        if (forest.empty()) {
            forest.init(n);
        } else {
            make_root(n);
        }
    }

    int pop() {
        assert(!empty());
        int n = top();
        forest.splice_children_back_unsafe(n);
        forest.clear(n);
        multi_pass_pairing();
        return n;
    }

    void improve(int n) {
        assert(0 <= n && n < forest.N && contains(n));
        if (!forest.is_root(n) && comp(n, forest.parent[n])) {
            make_root(n);
        }
    }
    void push_or_improve(int n) { contains(n) ? improve(n) : push(n); }

  private:
    void make_root(int n) {
        if (comp(n, top())) {
            forest.splice_front(forest.N, n);
        } else {
            forest.splice_back(forest.N, n);
        }
    }
    int meld(int u, int v) {
        if (comp(u, v)) {
            forest.splice_front(u, v);
            return u;
        } else {
            forest.splice_front(v, u);
            return v;
        }
    }
    void two_pass_pairing() {
        if (empty())
            return;
        int u = top(), v = forest.next[u];
        while (!forest.is_rep(u) && !forest.is_rep(v)) {
            u = forest.next[meld(u, v)], v = forest.next[u];
        }
        u = forest.tail(forest.N), v = forest.prev[u];
        while (!forest.is_rep(v)) {
            u = meld(u, v), v = forest.prev[u];
        }
        forest.parent[u] = forest.N;
    }
    void multi_pass_pairing() {
        if (empty())
            return;
        int u = top(), v = forest.next[u];
        while (!forest.is_rep(v)) {
            do {
                u = forest.next[meld(u, v)], v = forest.next[u];
            } while (!forest.is_rep(u) && !forest.is_rep(v));
            u = top(), v = forest.next[u];
        }
        forest.parent[u] = forest.N;
    }
};

#endif // INTEGER_DATA_STRUCTURES_HPP
