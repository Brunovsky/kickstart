#ifndef FAST_SIMPLE_HPP
#define FAST_SIMPLE_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Optimized versions of simple data structures like queues and lists for special
 * cases with known shape.
 *
 * Sources: https://github.com/indy256/codelibrary
 */

/**
 * Disjoint forward linked list over the integers [0...N).
 */
struct disjoint_forward_lists {
    vector<int> head, next;

    explicit disjoint_forward_lists(int N = 0, int M = 0) : head(N, -1), next(M) {}

    void push(int h, int n) { next[n] = head[h], head[h] = n; }
    void clear() { fill(begin(head), end(head), -1); }
    void resize(int N, int M) { head.resize(N, -1), next.resize(M); }
};

#define FOR_EACH_FORWARD_LIST(i, h, lists) \
    for (int i = lists.head[h]; i != -1; i = lists.next[i])

/**
 * Non-backtracking queue that holds at most N elements.
 */
template <typename T>
struct forward_queue {
    uint h = 0, t = 0;
    vector<T> data;

    explicit forward_queue(int N = 0, const T& elem = T()) : data(N, elem) {}

    void push(T elem) { assert(t < data.size()) data[t++] = move(elem); }
    T pop() { return assert(h < t), data[h++]; }
    void clear() { t = h = 0; }
    bool empty() const { return h == t; }
    int size() const { return t - h; }
    T& top() { return data[h]; }
    const T& top() const { return data[h]; }
};

/**
 * Circular queue that holds at most N-1 elements.
 */
template <typename T>
struct circular_queue {
    int N, h = 0, t = 0;
    vector<T> data;

    explicit circular_queue(int N = 0, const T& elem = T()) : N(N), data(N, elem) {}

    void push(T elem) { data[t++] = move(elem), t -= t == N ? N : 0; }
    T pop() { return assert(h != t), ++h == N ? (h = 0, data[N - 1]) : data[h - 1]; }
    void clear() { t = h = 0; }
    bool empty() const { return h == t; }
    int size() const { return h <= t ? t - h : t - h + N; }
    T& top() { return data[h]; }
    const T& top() const { return data[h]; }
};

#endif // FAST_SIMPLE_HPP
