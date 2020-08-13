#include <bits/stdc++.h>

using namespace std;

// *****

template <int MAXN>
struct fenwick {
    int N;
    int tree[MAXN + 1]{};

    fenwick() {}
    fenwick(int N) : N(N) {}

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

template <int MAXN, int MAXM>
struct fenwick2d {
    int N, M;
    int tree[MAXN + 1][MAXM + 1]{};

    fenwick2d() {}
    fenwick2d(int N, int M) : N(N), M(M) {}

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

// *****

void driver() { fenwick<30000> fw(30000); }
