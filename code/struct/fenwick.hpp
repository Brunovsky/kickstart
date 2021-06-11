#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * 1-indexed fenwick tree
 */
template <typename T>
struct fenwick {
    int N;
    vector<T> tree;

    explicit fenwick(int N = 0) : N(N), tree(N + 1) {}

    T sum(int i) const {
        T sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= i & -i;
        }
        return sum;
    }

    void add(int i, T n) {
        if (i > 0) {
            while (i <= N) {
                tree[i] += n;
                i += i & -i;
            }
        }
    }

    int lower_bound(T n) const {
        int i = 0;
        int bits = CHAR_BIT * sizeof(int) - __builtin_clz(N << 1);
        for (int j = 1 << bits; j; j >>= 1) {
            if (i + j <= N && tree[i + j] < n) {
                n -= tree[i + j];
                i += j;
            }
        }
        return i + 1;
    }
};

template <typename T>
struct sparse_fenwick {
    int N;
    unordered_map<int, T> tree;

    explicit sparse_fenwick(int N = 0) : N(N) {}

    T get(int i) const {
        auto it = tree.find(i);
        return it == tree.end() ? T() : it->second;
    }

    T sum(int i) const {
        T sum = 0;
        while (i > 0) {
            sum += get(i);
            i -= i & -i;
        }
        return sum;
    }

    void add(int i, T n) {
        if (i > 0) {
            while (i <= N) {
                tree[i] += n;
                i += i & -i;
            }
        }
    }

    int lower_bound(T n) const {
        int i = 0;
        int bits = CHAR_BIT * sizeof(int) - __builtin_clz(N << 1);
        for (int j = 1 << bits; j; j >>= 1) {
            if (i + j <= N && get(i + j) < n) {
                n -= get(i + j);
                i += j;
            }
        }
        return i + 1;
    }
};

// *****

/**
 * 1-indexed 2d fenwick tree
 */
template <typename T>
struct fenwick2d {
    int N, M;
    vector<vector<T>> tree;

    explicit fenwick2d(int N = 0, int M = 0)
        : N(N), M(M), tree(N + 1, vector<int>(M + 1)) {}

    T sum(int i, int j) const {
        T sum = 0;
        while (i > 0) {
            int k = j;
            while (k > 0) {
                sum += tree[i][k];
                k -= k & -k;
            }
            i -= i & -i;
        }
        return sum;
    }

    T rectangle(int i0, int j0, int i1, int j1) const {
        return sum(i1, j1) - sum(i0, j1) - sum(i1, j0) + sum(i0, j0);
    }

    void add(int i, int j, T n) {
        if (i > 0 && j > 0) {
            while (i <= N) {
                int k = j;
                while (k <= M) {
                    tree[i][k] += n;
                    k += k & -k;
                }
                i += i & -i;
            }
        }
    }
};

template <typename T>
struct sparse_fenwick2d {
    int N, M;
    unordered_map<pair<int, int>, T> tree;

    explicit sparse_fenwick2d(int N = 0, int M = 0) : N(N), M(M) {}

    T get(int i, int j) const {
        auto it = tree.find({i, j});
        return it == tree.end() ? T() : it->second;
    }

    T sum(int i, int j) const {
        T sum = 0;
        while (i > 0) {
            int k = j;
            while (k > 0) {
                sum += get(i, k);
                k -= k & -k;
            }
            i -= i & -i;
        }
        return sum;
    }

    T rectangle(int i0, int j0, int i1, int j1) const {
        return sum(i1, j1) - sum(i0, j1) - sum(i1, j0) + sum(i0, j0);
    }

    void add(int i, int j, T n) {
        if (i > 0 && j > 0) {
            while (i <= N) {
                int k = j;
                while (k <= M) {
                    tree[{i, k}] += n;
                    k += k & -k;
                }
                i += i & -i;
            }
        }
    }
};
