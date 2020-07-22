#include <bits/stdc++.h>

using namespace std;

// *****

#define QUERY(v) tree[v]
#define UPDATE(v, c) tree[v] += c
#define COMBINE(a, b) return (a) + (b)
#define PUSHUP(v) tree[v] = tree[(v) >> 1] + tree[(v) >> 1 | 1]

#define MAXN 100000
int N, tree[4 * MAXN];

void init(int arr[], int v, int tl, int tr) {
    if (tl == tr) {
        tree[v] = arr[tl];
    } else {
        int M = (tl + tr) / 2;
        init(arr, v << 1, tl, M);
        init(arr, v << 1 | 1, M + 1, tr);
        PUSHUP(v);
    }
}

auto query(int v, int tl, int tr, int ql, int qr) {
    if (tl > tr) {
        return 0;
    }
    if (ql == tl && qr == tr) {
        return QUERY(v);
    }
    int tm = (tl + tr) / 2;
    auto a = query(v << 1, tl, tm, ql, min(qr, tm));
    auto b = query(v << 1 | 1, tm + 1, tr, max(ql, tm + 1), qr);
    COMBINE(a, b);
}

void update(int v, int tl, int tr, int pos, int val) {
    if (tl == tr) {
        UPDATE(v, val);
    } else {
        int tm = (tl + tr) / 2;
        if (pos <= tm) {
            update(v << 1, tl, tm, pos, val);
        } else {
            update(v << 1 | 1, tm + 1, tr, pos, val);
        }
        PUSHUP(v);
    }
}

void driver() {
    N = MAXN;
    int arr[MAXN];
    iota(arr, arr + N, 0);
    memset(tree, 0, sizeof(tree));

    init(arr, 1, 0, N - 1);
}

// Simplified segment tree whose elementary ranges are 0, 1, ..., n - 1.
// Source: http://e-maxx.ru/algo/segment_tree
