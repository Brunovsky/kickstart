#ifndef FAST_SIMPLE_HPP
#define FAST_SIMPLE_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Optimized versions of simple data structures like queues and lists for special
 * cases with known shape.
 *
 * Ideas from: https://github.com/indy256/codelibrary
 */

/**
 * N Disjoint forward linked lists over the integers [0...M).
 * Caution: next[] is not reset by clear()
 */
struct disjoint_forward_lists {
    vector<int> head, next;

    explicit disjoint_forward_lists(int N = 0, int M = 0) : head(N, -1), next(M) {}

    void push(int l, int n) { next[n] = head[l], head[l] = n; }

    void clean(int l) { head[l] = -1; }
    bool empty(int l) const { return head[l] == -1; }
    void clear() { fill(begin(head), end(head), -1); }
    void resize(int N, int M) { head.resize(N, -1), next.resize(M); }
};

/**
 * N Disjoint doubly linked lists over the integers [0...M).
 * Caution: next[],prev[] are not reset by clear()
 */
struct disjoint_lists {
    vector<int> head, tail, next, prev;

    explicit disjoint_lists(int N = 0, int M = 0)
        : head(N, -1), tail(N, -1), next(M), prev(M) {}

    void insert_before(int i, int n) { meet_before(prev[i], n), meet(n, i); }
    void insert_after(int i, int n) { meet_after(n, next[i]), meet(i, n); }
    void push_front(int l, int n) {
        head[l] == -1 ? init(l, n) : (prev[n] = -1, meet(n, head[l]));
    }
    void push_back(int l, int n) {
        tail[l] == -1 ? init(l, n) : (next[n] = -1, meet(tail[l], n));
    }
    void splice_front(int l, int b) {
        assert(l != b);
        if (head[l] == -1)
            head[l] = head[b], tail[l] = tail[b];
        else if (tail[b] != -1)
            meet(tail[b], head[l]), head[l] = head[b];
        clean(b);
    }
    void splice_back(int l, int b) {
        assert(l != b);
        if (tail[l] == -1)
            head[l] = head[b], tail[l] = tail[b];
        else if (head[b] != -1)
            meet(tail[l], head[b]), tail[l] = tail[b];
        clean(b);
    }
    void splice_before(int i, int b) {
        if (head[b] != -1)
            meet_before(prev[i], head[b]), meet(tail[b], i), clean(b);
    }
    void splice_after(int i, int b) {
        if (head[b] != -1)
            meet_after(tail[b], next[i]), meet(i, head[b]), clean(b);
    }

    void clean(int l) { head[l] = tail[l] = -1; }
    bool empty(int l) const { return head[l] == -1; }
    void clear() {
        fill(begin(head), end(head), -1);
        fill(begin(tail), end(tail), -1);
    }
    void resize(int N, int M) {
        head.resize(N, -1), tail.resize(N, -1);
        next.resize(M), prev.resize(M);
    }

  private:
    inline void init(int l, int n) { head[l] = tail[l] = n, next[n] = prev[n] = -1; }
    inline void meet(int u, int v) {
        assert(u != v && u != -1 && v != -1);
        next[u] = v, prev[v] = u;
    }
    inline void meet_before(int u, int v) {
        assert(u != v && v != -1);
        if (u != -1)
            next[u] = v;
        prev[v] = u;
    }
    inline void meet_after(int u, int v) {
        assert(u != v && u != -1);
        if (v != -1)
            prev[v] = u;
        next[u] = v;
    }
};

#define FOR_EACH_IN_LIST(i, l, lists) \
    for (int i = lists.head[l]; i != -1; i = lists.next[i])

#define FOR_EACH_IN_LIST_REVERSE(i, l, lists) \
    for (int i = lists.tail[l]; i != -1; i = lists.prev[i])

/**
 * Non-backtracking queue that holds at most N elements, of any type.
 * At most N pushes can be performed before clear() is called.
 */
template <typename T>
struct forward_queue {
    uint h = 0, t = 0;
    vector<T> data;

    explicit forward_queue(int N = 0, const T& elem = T()) : data(N, elem) {}

    void push(T elem) { assert(t < data.size()), data[t++] = move(elem); }
    T pop() { return assert(h < t), data[h++]; }
    void clear() { t = h = 0; }
    bool empty() const { return h == t; }
    uint size() const { return t - h; }
    const T& top() const { return data[h]; }
};

/**
 * Circular queue that holds at most N-1 elements, of any type.
 */
template <typename T>
struct circular_queue {
    uint N, h = 0, t = 0;
    vector<T> data;

    explicit circular_queue(int N = 0, const T& elem = T()) : N(N), data(N, elem) {}

    void push(T elem) { data[t++] = move(elem), t -= t == N ? N : 0, assert(h != t); }
    T pop() { return assert(h != t), ++h == N ? (h = 0, data[N - 1]) : data[h - 1]; }
    void clear() { t = h = 0; }
    bool empty() const { return h == t; }
    uint size() const { return h <= t ? t - h : t + N - h; }
    const T& top() const { return data[h]; }
};

/**
 * Binary heap over the integers [0...M) with decrease-key.
 * By default a min-heap, but you'll usually need a custom compare (e.g. dijkstra).
 */
template <typename Compare = less<>>
struct binary_heap {
    vector<int> c, idx;
    Compare comp;

    explicit binary_heap(int M = 0, const Compare& comp = Compare())
        : c(1, 0), idx(M, 0), comp(comp) {}

    bool empty() const { return c.size() == 1u; }
    uint size() const { return c.size() - 1; }
    bool contains(int n) const { return idx[n] != 0; }

    int top() const {
        assert(!empty());
        return c[1];
    }

    void push(int n) {
        assert(!contains(n));
        idx[n] = c.size(), c.push_back(n);
        heapify_up(idx[n]);
    }

    int pop() {
        assert(!empty());
        int n = c[1];
        swap(c[1], c.back());
        idx[c[1]] = 1, idx[n] = 0;
        c.pop_back();
        heapify_down(1);
        return n;
    }

    void improve(int n) { assert(contains(n)), heapify_up(idx[n]); }
    void decline(int n) { assert(contains(n)), heapify_down(idx[n]); }
    void push_or_improve(int n) { contains(n) ? improve(n) : push(n); }
    void push_or_decline(int n) { contains(n) ? decline(n) : push(n); }

  private:
    void heapify_up(int i) {
        while (i > 1 && comp(c[i], c[i >> 1])) // while c[i] < c[parent(i)]
            exchange(i, i >> 1), i >>= 1;
    }

    void heapify_down(int i) {
        int k, S = c.size();
        while ((k = i << 1) < S) {
            if (k + 1 < S && !comp(c[k], c[k + 1])) // c[k + 1] <= c[k]
                k++;
            if (!comp(c[k], c[i])) // break if c[i] <= c[k]
                break;
            exchange(i, k), i = k;
        }
    }

    void exchange(int i, int j) { swap(idx[c[i]], idx[c[j]]), swap(c[i], c[j]); }
};

#endif // FAST_SIMPLE_HPP
