#ifndef INTEGER_HEAPS_HPP
#define INTEGER_HEAPS_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

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
    int top() const { return assert(!empty()), c[0]; }

    void push(int u) {
        assert(!contains(u));
        id[u] = c.size(), c.push_back(u);
        heapify_up(id[u]);
    }
    int pop() {
        assert(!empty());
        int u = c[0];
        swap(c[0], c.back());
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
        for (int u : c)
            id[u] = -1;
        c.clear();
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
 *           O(n)            clear()
 *
 * Usage:
 *      vector<int> dist(V); ...
 *      pairing_int_heap heap(V, greater_container(dist));
 *      heap.push(0);                  -- push 0 onto heap
 *      heap.push_or_improve(1);       -- dec. key of 1 (or push)
 *      heap.improve(0);               -- dec. key of 0, 0 must've been pushed
 *      int v = heap.top();            -- top of heap
 *      heap.clear();                  -- clear the whole heap (not recommended)
 *      v = heap.pop(0);               -- pop from heap (returns value)
 */
template <typename Compare = less<>>
struct pairing_int_heap {
    struct node_t {
        int parent = 0, child = 0, next = 0, prev = 0;
    };
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
        root = empty() ? u : meld(root, u);
    }
    int pop() {
        assert(!empty());
        int u = root;
        root = two_pass_pairing(u);
        node[u] = node_t();
        return u - 1;
    }
    void improve(int u) {
        assert(!empty() && contains(u)), u++;
        if (u != root && do_comp(u, node[u].parent)) {
            take(u), root = meld(root, u);
        }
    }
    void push_or_improve(int u) { contains(u) ? improve(u) : push(u); }

    void clear() {
        if (!empty()) {
            clear_dive(root), root = 0;
        }
    }

  private:
    bool do_comp(int u, int v) const { return comp(u - 1, v - 1); }
    int meld(int u, int v) { return do_comp(u, v) ? splice(u, v) : splice(v, u); }
    int splice(int u, int v) {
        node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent > 0);
        if (node[node[u].parent].child == u)
            node[node[u].parent].child = node[u].next;
        else
            node[node[u].prev].next = node[u].next;
        node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == 0)
            return 0;
        int u = node[n].child, v = node[u].next, w;
        while (v && node[v].next) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v) {
            w = node[v].next, u = meld(u, v), v = w;
        }
        return u;
    }
    void clear_dive(int u) {
        for (int v = node[u].child; v; v = node[v].next)
            clear_dive(v);
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
    };
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
        root[h] = empty(h) ? u : meld(root[h], u);
    }
    int pop(int h) {
        assert(!empty(h));
        int u = root[h];
        root[h] = two_pass_pairing(u);
        node[u] = node_t();
        return u - 1;
    }
    void improve(int h, int u) {
        assert(!empty(h) && contains(u)), u++;
        if (u != root[h] && do_comp(u, node[u].parent)) {
            take(u), root[h] = meld(root[h], u);
        }
    }
    void push_or_improve(int h, int u) { contains(u) ? improve(h, u) : push(h, u); }

    void merge(int h, int g) {
        if (h != g && !empty(g)) {
            root[h] = empty(h) ? root[g] : meld(root[h], root[g]);
            root[g] = 0;
        }
    }
    void clear(int h) {
        if (!empty(h)) {
            clear_dive(root[h]), root[h] = 0;
        }
    }

  private:
    bool do_comp(int u, int v) const { return comp(u - 1, v - 1); }
    int meld(int u, int v) { return do_comp(u, v) ? splice(u, v) : splice(v, u); }
    int splice(int u, int v) {
        node[node[u].child].prev = v;
        node[v].next = node[u].child, node[u].child = v;
        return node[v].prev = node[v].parent = u;
    }
    void take(int u) {
        assert(node[u].parent > 0);
        if (node[node[u].parent].child == u)
            node[node[u].parent].child = node[u].next;
        else
            node[node[u].prev].next = node[u].next;
        node[node[u].next].prev = node[u].prev;
    }
    int two_pass_pairing(int n) {
        if (node[n].child == 0)
            return 0;
        int u = node[n].child, v = node[u].next, w;
        while (v && node[v].next) {
            w = node[node[v].next].next;
            u = node[u].next = v = meld(v, node[v].next);
            v = node[v].next = w;
        }
        u = node[n].child, v = node[u].next;
        while (v) {
            w = node[v].next, u = meld(u, v), v = w;
        }
        return u;
    }
    void clear_dive(int u) {
        for (int v = node[u].child; v; v = node[v].next)
            clear_dive(v);
        node[u] = node_t();
    }
};

template <typename Container>
struct less_container {
    const Container& cont;
    explicit less_container(const Container& cont) : cont(cont) {}
    inline bool operator()(int u, int v) const { return cont[u] < cont[v]; }
};

template <typename Container>
struct greater_container {
    const Container& cont;
    explicit greater_container(const Container& cont) : cont(cont) {}
    inline bool operator()(int u, int v) const { return cont[u] > cont[v]; }
};

#endif // INTEGER_HEAPS_HPP
