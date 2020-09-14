#ifndef HEAP_HPP
#define HEAP_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Binary heap for integers [0...N) with decrease and increase-key support.
 */
template <typename Compare = less<>>
struct binary_heap {
    vector<int> c, idx;
    Compare comp;

    explicit binary_heap(int N = 0, const Compare& comp = Compare())
        : c(0, 0), idx(N, 0), comp(comp) {}

    bool empty() const { return c.empty(); }
    size_t size() const { return c.size(); }
    bool contains(int n) const { return idx[n] != -1; }
    int top() const { return assert(!empty()), c[0]; }

    void push(int n) {
        assert(!contains(n));
        idx[n] = c.size(), c.push_back(n);
        heapify_up(idx[n]);
    }

    int pop() {
        assert(!empty());
        int n = c[0];
        swap(c[0], c.back());
        idx[c[0]] = 0, idx[n] = -1;
        c.pop_back();
        heapify_down(0);
        return n;
    }

    void improve(int n) { assert(contains(n)), heapify_up(idx[n]); }
    void decline(int n) { assert(contains(n)), heapify_down(idx[n]); }
    void push_or_improve(int n) { contains(n) ? improve(n) : push(n); }
    void push_or_decline(int n) { contains(n) ? decline(n) : push(n); }

  private:
    void heapify_up(int i) {
        while (i > 0 && comp(c[i], c[(i - 1) >> 1])) { // c[i] < c[parent(i)]
            exchange(i, (i - 1) >> 1), i = (i - 1) >> 1;
        }
    }

    void heapify_down(int i) {
        int k = i << 1 | 1, s = c.size();
        while (k < s) {
            if (k + 1 < s && !comp(c[k], c[k + 1]))
                k++;
            if (!comp(c[k], c[i]))
                break;
            exchange(i, k), i = k, k = i << 1 | 1;
        }
    }

    void exchange(int i, int j) { swap(idx[c[i]], idx[c[j]]), swap(c[i], c[j]); }
};

#endif // HEAP_HPP
