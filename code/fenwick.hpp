#ifndef FENWICK_HPP
#define FENWICK_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

struct fenwick {
    int N;
    vector<int> tree;

    explicit fenwick(int N = 0) : N(N), tree(N + 1, 0) {}

    int sum(int i) {
        int sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= i & -i;
        }
        return sum;
    }

    void add(int i, int n) {
        assert(i > 0);
        while (i <= N) {
            tree[i] += n;
            i += i & -i;
        }
    }

    int lower_bound(int n) {
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

// *****

struct fenwick2d {
    int N, M;
    vector<vector<int>> tree;

    explicit fenwick2d(int N = 0, int M = 0)
        : N(N), M(M), tree(N + 1, vector<int>(M + 1, 0)) {}

    int sum(int i, int j) {
        int sum = 0;
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

    void add(int i, int j, int n) {
        assert(i > 0 && j > 0);
        while (i <= N) {
            int k = j;
            while (k <= M) {
                tree[i][k] += n;
                k += k & -k;
            }
            i += i & -i;
        }
    }
};

#endif // FENWICK_HPP
