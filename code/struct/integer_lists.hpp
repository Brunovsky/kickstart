#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Numbered collection of forward linked lists over the integers [0...N).
 * Each list maintains a head pointer and each int maintains a next pointer.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next pointers are not reset by clear()
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
 * Numbered collection of L doubly linked lists [0..L) over the integers [0...N)
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

/**
 * A version of linked_lists where you can add and remove nodes at will, but not lists
 * The nodes appear after the lists, increasing the constant factor slightly
 * Appropriate if you want something like linked_lists(V, E) but E is dynamic.
 */
struct dynamic_linked_lists {
    int L, N;
    vector<int> next, prev, freelist;

    // L: lists are [0...L), N: integers are [0...N)
    explicit dynamic_linked_lists(int L = 0, int N = 0) { assign(L, N); }

    int pos(int n) const { return n + L; } // representative node of number n
    int succ(int n) const { return next[pos(n)]; }
    int pred(int n) const { return prev[pos(n)]; }
    bool empty(int l) const { return next[l] == l; }

    int add_element() {
        if (freelist.empty()) {
            return next.push_back(N), prev.push_back(N), N++;
        } else {
            int u = freelist.back();
            return freelist.pop_back(), u;
        }
    }
    void rem_element(int n) { freelist.push_back(n), prev[pos(n)] = next[pos(n)] = n; }
    void clear(int l) { assert(0 <= l && l < L), next[l] = prev[l] = l; }

    void init(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(l, pos(n), l);
    }
    void push_front(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(l, pos(n), next[l]);
    }
    void push_back(int l, int n) {
        assert(0 <= l && l < L && 0 <= n && n < N), meet(prev[l], pos(n), l);
    }
    void insert_before(int i, int n) {
        assert(0 <= i && i < N && 0 <= n && n < N), meet(pred(i), pos(n), pos(i));
    }
    void insert_after(int i, int n) {
        assert(0 <= i && i < N && 0 <= n && n < N), meet(pos(i), pos(n), succ(i));
    }
    void erase(int n) {
        assert(0 <= n && n < N), meet(pred(n), succ(n)); //
    }
    void pop_front(int l) {
        assert(0 <= l && l < L && !empty(l)), meet(l, next[next[l]]);
    }
    void pop_back(int l) {
        assert(0 <= l && l < L && !empty(l)), meet(prev[prev[l]], l); //
    }

    void splice_front(int l, int b) {
        assert(0 <= l && l < L && 0 <= b && b < L);
        if (l != b && !empty(b))
            meet(prev[b], next[l]), meet(l, next[b]), clear(b);
    }
    void splice_back(int l, int b) {
        assert(0 <= l && l < L && 0 <= b && b < L);
        if (l != b && !empty(b))
            meet(prev[l], next[b]), meet(prev[b], l), clear(b);
    }

    void clear() {
        iota(begin(next), begin(next) + L, 0);
        iota(begin(prev), begin(prev) + L, 0);
        freelist.clear();
    }
    void assign(int L, int N) {
        this->L = L, this->N = N;
        next.resize(L + N), prev.resize(L + N), clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

#define FOR_EACH_IN_DYN_LINKED_LIST(i, l, lists) \
    for (int z##i = l, i = lists.next[z##i]; i != z##i; i = lists.succ(i))

#define FOR_EACH_IN_DYN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int z##i = l, i = lists.prev[z##i]; i != z##i; i = lists.pred(i))
