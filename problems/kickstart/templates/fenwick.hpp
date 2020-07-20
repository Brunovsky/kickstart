#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100001

int N;
int fenwick[MAXN];

int sum(int i) {
    int sum = 0;
    while (i > 0) {
        sum += fenwick[i];
        i -= i & -i;
    }
    return sum;
}

void add(int i, int n) {
    assert(i > 0);
    while (i <= N) {
        fenwick[i] += n;
        i += i & -i;
    }
}

// *****

#define MAXM 500

int M;
int fenwick2d[MAXN][MAXM];

int sum2(int i, int j) {
    int sum = 0;
    while (i > 0) {
        int k = j;
        while (k > 0) {
            sum += fenwick2d[i][k];
            k -= k & -k;
        }
        i -= i & -i;
    }
    return sum;
}

void add2(int i, int j, int n) {
    assert(i > 0 && j > 0);
    while (i <= N) {
        int k = j;
        while (k <= N) {
            fenwick2d[i][k] += n;
            k += k & -k;
        }
        i += i & -i;
    }
}

// *****

void driver() {
    memset(fenwick, 0, sizeof(fenwick));
    memset(fenwick2d, 0, sizeof(fenwick2d));
}
