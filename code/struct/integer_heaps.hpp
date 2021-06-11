#pragma once

#include <bits/stdc++.h>
using namespace std;

template <typename Container>
struct less_container {
    const Container* cont = nullptr;
    less_container() = default;
    less_container(const Container& cont) : cont(&cont) {}
    inline bool operator()(int u, int v) const {
        return tie((*cont)[u], u) < tie((*cont)[v], v);
    }
};

template <typename Container>
struct greater_container {
    const Container* cont = nullptr;
    greater_container() = default;
    greater_container(const Container& cont) : cont(&cont) {}
    inline bool operator()(int u, int v) const {
        return tie((*cont)[u], u) > tie((*cont)[v], v);
    }
};

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
 *           O(n)            clear()
 */
template <typename Compare = less<>>
struct binary_int_heap {
    vector<int> c, id;
    Compare comp;

    explicit binary_int_heap(int N = 0, const Compare& comp = Compare())
        : c(0, 0), id(N, -1), comp(comp) {}

    bool empty() const { return c.empty(); }
    size_t size() const { return c.size(); }
    bool contains(int u) const { return id[u] != -1; }
    int top() const {
        assert(!empty());
        return c[0];
    }
    void push(int u) {
        assert(!contains(u));
        id[u] = c.size(), c.push_back(u);
        heapify_up(id[u]);
    }
    int pop() {
        assert(!empty());
        int u = c[0];
        c[0] = c.back();
        id[c[0]] = 0, id[u] = -1;
        c.pop_back();
        heapify_down(0);
        return u;
    }
    void improve(int u) { assert(contains(u)), heapify_up(id[u]); }
    void decline(int u) { assert(contains(u)), heapify_down(id[u]); }
    void push_or_improve(int u) { contains(u) ? improve(u) : push(u); }
    void push_or_decline(int u) { contains(u) ? decline(u) : push(u); }
    void clear() {
        for (int u : c) id[u] = -1;
        c.clear();
    }
    void fill() {
        for (int u = 0, N = id.size(); u < N; u++) {
            if (!contains(u)) { push(u); }
        }
    }

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
 * Unfortunately slower than binary_int_heap in practice, unless you need the
 * special operations.
 *
 * Usage:
 *      vector<int> dist(V); ...
 *      pairing_int_heap heap(V, greater_container(dist));
 *      heap.push(0);                  -- push 0 onto heap
 *      heap.push_or_improve(1);       -- dec. key of 1 (or push)
 *      heap.improve(0);               -- dec. key of 0, 0 must've been pushed
 *      heap.adjust(2);                -- dec. or inc. key of 2 (erase+push)
 *      heap.fill();                   -- push all missing integers
 *      int v = heap.top();            -- top of heap
 *      v = heap.pop(0);               -- pop from heap (returns value)
 *      heap.clear();                  -- clear the whole heap (not recommended)
 */
template <typename Compare = less<>>
struct pairing_int_heap {
    struct node_t {
        int parent = 0, child = 0, next = 0, prev = 0;
    }; // elements are shifted by 1 to allow 0 to be used as a scratchpad
    vector<node_t> node;
    int root = 0;
    Compare comp;

    explicit pairing_int_heap(int N = 0, const Compare& comp = Compare())
        : node(N + 1), comp(comp) {}

    bool empty() const { return root == 0; }
    bool contains(int u) const { return u++, node[u].parent != 0; }
    int top() const { return root - 1; }

    void push(int u) {
        assert(!contains(u)), u++;
        node[u].parent = -1;
        root = safe_meld(root, u);
    }
    int pop() {
        assert(!empty());
        int u = root;
        root = two_pass_pairing(u);
        node[root].parent = -1;
        node[u] = node_t();
        return u - 1;
    }
    void improve(int u) {
        assert(!empty() && contains(u)), u++;
        if (u != root && do_comp(u, node[u].parent)) { take(u), root = meld(root, u); }
    }
    void push_or_improve(int u) {
        if (contains(u)) {
            improve(u);
        } else {
            push(u);
        }
    }
    void adjust(int u) {
        erase(u);
        push(u);
    }
    void erase(int u) {
        assert(contains(u)), u++;
        if (u == root) {
            pop();
        } else {
            take(u);
            int v = two_pass_pairing(u);
            root = safe_meld(root, v);
            node[root].parent = -1;
            node[u] = node_t();
        }
    }
    void clear() {
        if (!empty()) { clear_rec(root), root = 0; }
    }
    void fill() {
        for (int u = 1, N = node.size() - 1; u <= N; u++) {
            if (!contains(u)) { push(u); }
        }
    }

  private:
    bool do_comp(int u, int v) const { return comp(u - 1, v - 1); }
    int meld(int u, int v) { return do_comp(u, v) ? splice(u, v) : splice(v, u); }
    int safe_meld(int u, int v) {
        if (u == 0 || v == 0 || u == v) return u ? u : v;
        return meld(u, v);
    }
    int splice(int u, int v) {
        node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent > 0);
        if (node[node[u].parent].child == u) {
            node[node[u].parent].child = node[u].next;
        } else {
            node[node[u].prev].next = node[u].next;
        }
        node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == 0) return 0;
        int u = node[n].child, v = node[u].next, w;
        while (v && node[v].next) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v) { w = node[v].next, u = meld(u, v), v = w; }
        return u;
    }
    void clear_rec(int u) {
        for (int v = node[u].child, w = node[v].next; v; v = w, w = node[v].next) {
            clear_rec(v);
        }
        node[u] = node_t();
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
 *           O(n)            clear(h)
 *
 * Usage:
 *      vector<int> dist(V); ...
 *      pairing_int_heaps heaps(N, V, greater_container(dist));
 *      heaps.push(1, 0);               -- push 0 onto heap 1
 *      heaps.push_or_improve(2, 1);    -- dec. key of 1 in heap 2 (or push)
 *      heaps.improve(1, 0);            -- dec. key of 0 in heap 1, 0 must've been pushed
 *      int v = heaps.top(0);           -- top of heap 0
 *      heaps.merge(1, 2);              -- merge heap 2 into heap 1 (then heap 2 is empty)
 *      heaps.clear(1);                 -- clear heap 1 (not recommended)
 *      v = heaps.pop(0);               -- pop from heap 0 (returns value)
 */
template <typename Compare = less<>>
struct pairing_int_heaps {
    struct node_t {
        int parent = 0, child = 0, next = 0, prev = 0;
    }; // elements are shifted by 1 to allow 0 to be used as a scratchpad
    vector<int> root;
    vector<node_t> node;
    Compare comp;

    explicit pairing_int_heaps(int R = 0, int N = 0, const Compare& comp = Compare())
        : root(R, 0), node(N + 1), comp(comp) {}

    bool empty(int h) const { return root[h] == 0; }
    bool contains(int u) const { return u++, node[u].parent != 0; }
    int top(int h) const { return root[h] - 1; }

    void push(int h, int u) {
        assert(!contains(u)), u++;
        node[u].parent = -1;
        root[h] = safe_meld(root[h], u);
    }
    int pop(int h) {
        assert(!empty(h));
        int u = root[h];
        root[h] = two_pass_pairing(u);
        node[root[h]].parent = -1;
        node[u] = node_t();
        return u - 1;
    }
    void improve(int h, int u) {
        assert(!empty(h) && contains(u)), u++;
        if (u != root[h] && do_comp(u, node[u].parent)) {
            take(u), root[h] = meld(root[h], u);
        }
    }
    void push_or_improve(int h, int u) {
        if (contains(u)) {
            improve(h, u);
        } else {
            push(h, u);
        }
    }
    void adjust(int h, int u) {
        erase(h, u);
        push(h, u);
    }
    void erase(int h, int u) {
        assert(!empty(h) && contains(u)), u++;
        if (u == root[h]) {
            pop();
        } else {
            take(u);
            int v = two_pass_pairing(u);
            root[h] = safe_meld(root[h], v);
            node[root[h]].parent = -1;
            node[u] = node_t();
        }
    }
    void merge(int h, int g) {
        int r = safe_meld(root[h], root[g]);
        root[g] = 0, root[h] = r;
    }
    void clear(int h) {
        if (!empty(h)) { clear_rec(root[h]), root[h] = 0; }
    }
    void fill(int h) {
        for (int u = 1, N = node.size() - 1; u <= N; u++) {
            if (!contains(u)) { push(h, u); }
        }
    }
    void fill_each() {
        assert(root.size() + 1 == node.size());
        for (int h = 0, u = 1, N = node.size() - 1; u <= N; h++, u++) {
            if (!contains(u)) { push(h, u); }
        }
    }

  private:
    bool do_comp(int u, int v) const { return comp(u - 1, v - 1); }
    int meld(int u, int v) { return do_comp(u, v) ? splice(u, v) : splice(v, u); }
    int safe_meld(int u, int v) {
        if (u == 0 || v == 0 || u == v) return u ? u : v;
        return meld(u, v);
    }
    int splice(int u, int v) {
        node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent > 0);
        if (node[node[u].parent].child == u) {
            node[node[u].parent].child = node[u].next;
        } else {
            node[node[u].prev].next = node[u].next;
        }
        node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == 0) return 0;
        int u = node[n].child, v = node[u].next, w;
        while (v && node[v].next) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v) { w = node[v].next, u = meld(u, v), v = w; }
        return u;
    }
    void clear_rec(int u) {
        for (int v = node[u].child, w = node[v].next; v; v = w, w = node[v].next) {
            clear_rec(v);
        }
        node[u] = node_t();
    }
};

/**
 * A skew heap designed specifically for the minimum arborescence problem.
 * Might also be applicable to other problems requiring connected component contraction.
 *
 * Context: you want to represent a group of connected components in a graph with V nodes
 * and E edges, where edges contain values/costs and want to support the operation of
 * merging connected components efficiently, adding new edges, popping min edges,
 * and adding lazily a value to all edges in a subcomponent.
 *
 * Internally there are V "heaps" and E "nodes", which represent, respectively, the
 * represented graph's nodes and edges. The heaps correspond to connected components.
 */
template <typename T, typename Compare = less<>>
struct lazy_skew_int_heaps {
    struct node_t {
        int child[2] = {};
        T cost = {}, lazy = {};
    }; // elements are shifted by 1 to allow 0 to be used as a scratchpad
    vector<int> root;
    vector<node_t> node;
    Compare comp;

    explicit lazy_skew_int_heaps(int R = 0, int E = 0, const Compare& comp = Compare())
        : root(R), node(E + 1), comp(comp) {}

    bool empty(int h) const { return root[h] == 0; }
    auto top(int h) {
        pushdown(root[h]);
        return make_pair(root[h] - 1, node[root[h]].cost);
    }
    void update(int h, T delta) {
        assert(!empty(h));
        node[root[h]].lazy += delta;
        pushdown(root[h]);
    }
    void push(int h, int u, T cost) {
        assert(u >= 0), u++;
        node[u].cost = cost;
        root[h] = safe_meld(root[h], u);
    }
    void pop(int h) {
        assert(!empty(h));
        pushdown(root[h]);
        auto [l, r] = node[root[h]].child;
        node[root[h]] = node_t();
        root[h] = safe_meld(l, r);
    }
    void merge(int h, int a, int b) { // merge heaps a and b into position h
        assert(h == a || h == b || root[h] == 0);
        int r = safe_meld(root[a], root[b]);
        root[a] = root[b] = 0, root[h] = r;
    }

  private:
    void pushdown(int a) {
        auto [l, r] = node[a].child;
        node[a].cost += node[a].lazy;
        node[l].lazy += node[a].lazy;
        node[r].lazy += node[a].lazy;
        node[a].lazy = node[0].lazy = 0;
    }
    int safe_meld(int u, int v) {
        if (u == 0 || v == 0 || u == v) return u ? u : v;
        return meld(u, v);
    }
    int meld(int a, int b) {
        pushdown(a), pushdown(b);
        if (comp(node[b].cost, node[a].cost)) { swap(a, b); }
        swap(node[a].child[0], node[a].child[1] = safe_meld(b, node[a].child[1]));
        return a;
    }
};
