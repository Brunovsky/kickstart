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
    void clear() {
        if (root != -1)
            clear_dive(root), root = -1;
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
    void clear_dive(int u) {
        for (int v = node[u].child; v != -1; v = node[v].next)
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
    void clear(int h) {
        if (root[h] != -1)
            clear_dive(root[h]), root[h] = -1;
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
    void clear_dive(int u) {
        for (int v = node[u].child; v != -1; v = node[v].next)
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
