#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 5000

int N;
int fenwick[MAXN][MAXN];

int prefix_sum(int i, int j) {
    int sum = 0;
    while (i > 0) {
        int k = j;
        while (k > 0) {
            sum += fenwick[i][k];
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
        while (k <= N) {
            fenwick[i][k] += n;
            k += k & -k;
        }
        i += i & -i;
    }
}

// *****

void driver() {
    memset(fenwick, 0, sizeof(fenwick));
}
