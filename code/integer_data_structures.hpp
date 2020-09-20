#ifndef INTEGER_DATA_STRUCTURES_HPP
#define INTEGER_DATA_STRUCTURES_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Optimized versions of simple data structures like trees, queues and lists for special
 * case over the integers [0...M).
 * Parent/child/list relationships are kept in vectors instead of pointers.
 *
 * Ideas from: https://github.com/indy256/codelibrary
 */

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
 *     bucket sorting
 *     list of stacks with LIFO semantics
 *       fl.push(s, n)                     -- add value n to set s
 *       n = fl.head(s)                    -- read the top of the set s
 *       fl.pop(s)                         -- pop the top of the set s
 *       fl.empty(s)                       -- check whether set s is empty
 *       FOR_EACH_IN_FORWARD_LIST(i,s,fl)  -- iterate the elements of the set
 *       fl.clear(s)                       -- clear the set s after iterating it
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
 *     bucket sorting
 *     list of stacks with FIFO semantics
 *     list of sets with positional insert and erase
 *       fl.push_back(s, n)                -- add value n to set s
 *       n = fl.head(s)                    -- read the top of the set s
 *       fl.pop_front(s)                   -- pop the top of the set s
 *       fl.empty(s)                       -- check whether set s is empty
 *       FOR_EACH_IN_LINKED_LIST(i,s,fl)   -- iterate forward the elements of the set s
 *       fl.clear(s)                       -- clear the set s after iterating it
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
        assert(!empty(l) && !empty(t));
        int a = next[rep(l)], b = next[rep(t)], c = prev[rep(l)], d = prev[rep(t)];
        meet(rep(t), a), meet(rep(l), b), meet(c, rep(t)), meet(d, rep(l));
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
 * Binary heap over the integers [0...N) with decrease-key.
 * By default a min-heap, but you'll usually need a custom compare (e.g. dijkstra).
 *
 * Operation complexities (n is the size of the heap, u is a value)
 *           O(N)            construction
 *           O(1)            top()
 *           O(1)            contains(u)
 *           O(log n)        push(u)
 *           O(log n)-       decrease_key(u)
 *           O(log n)        pop()
 *           O(log n)        erase(u)     not implemented
 *           O(n)            clear()      not implemented
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
    int top() const { return assert(!empty()), c[0]; }

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
    static int parent(int i) { return (i - 1) >> 1; }
    static int child(int i) { return i << 1 | 1; }
    void exchange(int i, int j) { swap(id[c[i]], id[c[j]]), swap(c[i], c[j]); }
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
};

/**
 * Pairing heap over the unique integers [0...N)
 * By default a min-heap, but you'll usually need a custom compare.
 * Unfortunately slower than binary_int_heap in practice.
 *
 * Operation complexities (n is the size of the heap, u is a value)
 *           O(N)            construction
 *           O(1)            top()
 *           O(1)            contains(u)
 *           O(1)            push(u)
 *           O(log log n)*   decrease_key(u)
 *           O(log n)*       pop()
 *           O(log n)*       erase(u)     not implemented
 *           O(n)            clear()      not implemented
 */
template <typename Compare = less<>>
struct pairing_int_heap {
    struct node_t {
        int parent = -2, child = -1, next = -1, prev = -1;
    };
    vector<node_t> node;
    int root = -1;
    Compare comp;

    explicit pairing_int_heap(int N = 0, const Compare& comp = Compare())
        : node(N), comp(comp) {}

    bool empty() const { return root == -1; }
    bool contains(int u) const { return node[u].parent != -2; }
    int top() const { return assert(!empty()), root; }

    void push(int u) {
        assert(!contains(u));
        node[u].parent = -1;
        root = empty() ? u : meld(root, u);
    }
    int pop() {
        assert(!empty());
        int u = root;
        root = two_pass_pairing(u);
        node[u] = node_t();
        return u;
    }
    void improve(int u) {
        assert(!empty() && contains(u));
        if (u != root && comp(u, node[u].parent))
            take(u), root = meld(root, u);
    }
    void push_or_improve(int u) { contains(u) ? improve(u) : push(u); }

  private:
    int meld(int u, int v) { return comp(u, v) ? splice(u, v) : splice(v, u); }
    int splice(int u, int v) {
        if (node[u].child != -1)
            node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent != -1);
        if (node[node[u].parent].child == u)
            node[node[u].parent].child = node[u].next;
        else
            node[node[u].prev].next = node[u].next;
        if (node[u].next != -1)
            node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == -1)
            return -1;
        int u = node[n].child, v = node[u].next, w;
        while (v != -1 && node[v].next != -1) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v != -1) {
            w = node[v].next, u = meld(u, v), v = w;
        }
        return u;
    }
};

/**
 * Collection of R mergeable pairing heaps over the unique integers [0...N)
 * By default min-heaps, but you'll usually need a custom compare.
 *
 * Operation complexities (h,g are heaps, n is the size of the heap h, u is a value)
 *           O(R + N)        construction
 *           O(1)            top(h)
 *           O(1)            contains(u)
 *           O(1)            push(h,u)
 *           O(1)            merge(h,g)
 *           O(log log n)*   decrease_key(h,u)
 *           O(log n)*       pop(h)
 *           O(log n)*       erase(n)     not implemented
 *           O(n)            clear(h)     not implemented
 */
template <typename Compare = less<>>
struct pairing_int_heaps {
    struct node_t {
        int parent = -2, child = -1, next = -1, prev = -1;
    };
    vector<int> root;
    vector<node_t> node;
    Compare comp;

    explicit pairing_int_heaps(int R = 0, int N = 0, const Compare& comp = Compare())
        : root(R, -1), node(N), comp(comp) {}

    bool empty(int h) const { return root[h] == -1; }
    bool contains(int u) const { return node[u].parent != -2; }
    int top(int h) const { return assert(!empty(h)), root[h]; }

    void push(int h, int u) {
        assert(!contains(u));
        node[u].parent = -1;
        root[h] = empty(h) ? u : meld(root[h], u);
    }
    int pop(int h) {
        assert(!empty(h));
        int u = root[h];
        root[h] = two_pass_pairing(u);
        node[u] = node_t();
        return u;
    }
    void improve(int h, int u) {
        assert(!empty(h) && contains(u));
        if (u != root[h] && comp(u, node[u].parent))
            take(u), root[h] = meld(root[h], u);
    }
    void push_or_improve(int h, int u) { contains(u) ? improve(h, u) : push(h, u); }
    void merge(int h, int g) {
        if (h != g && !empty(g))
            root[h] = empty(h) ? root[g] : meld(root[h], root[g]), root[g] = -1;
    }

  private:
    int meld(int u, int v) { return comp(u, v) ? splice(u, v) : splice(v, u); }
    int splice(int u, int v) {
        if (node[u].child != -1)
            node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent != -1);
        if (node[node[u].parent].child == u)
            node[node[u].parent].child = node[u].next;
        else
            node[node[u].prev].next = node[u].next;
        if (node[u].next != -1)
            node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == -1)
            return -1;
        int u = node[n].child, v = node[u].next, w;
        while (v != -1 && node[v].next != -1) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v != -1) {
            w = node[v].next, u = meld(u, v), v = w;
        }
        return u;
    }
};

template <typename Container>
auto less_container(const Container& cont) {
    return [&cont](int u, int v) { return cont[u] < cont[v]; };
}
template <typename Container>
auto greater_container(const Container& cont) {
    return [&cont](int u, int v) { return cont[u] > cont[v]; };
}

#endif // INTEGER_DATA_STRUCTURES_HPP
