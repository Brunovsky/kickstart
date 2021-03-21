#ifndef SIMPLE_DATA_STRUCTURES_HPP
#define SIMPLE_DATA_STRUCTURES_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Non-backtracking queue that holds at most N elements, of any type.
 * At most N pushes can be performed before clear() is called.
 */
template <typename T>
struct forward_queue {
    uint h = 0, t = 0;
    vector<T> data;

    explicit forward_queue(int N = 0, const T& elem = T()) : data(N, elem) {}

    bool empty() const { return h == t; }
    uint size() const { return t - h; }
    const T& top() const { return data[h]; }
    void clear() { t = h = 0; }

    void push(T elem) { assert(t < data.size()), data[t++] = move(elem); }
    T pop() { return assert(h < t), data[h++]; }
};

/**
 * Circular queue that holds at most N-1 elements, of any type.
 */
template <typename T>
struct circular_queue {
    uint N, h = 0, t = 0;
    vector<T> data;

    explicit circular_queue(int N = 0, const T& elem = T()) : N(N), data(N, elem) {}

    bool empty() const { return h == t; }
    uint size() const { return h <= t ? t - h : t + N - h; }
    const T& top() const { return data[h]; }
    void clear() { t = h = 0; }

    void push(T elem) { data[t++] = move(elem), t -= t == N ? N : 0, assert(h != t); }
    T pop() { return assert(h != t), ++h == N ? (h = 0, data[N - 1]) : data[h - 1]; }
};

#endif // SIMPLE_DATA_STRUCTURES_HPP
