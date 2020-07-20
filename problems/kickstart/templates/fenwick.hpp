#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100001

int N;
int fenwick[MAXN];

int prefix_sum(int i) {
    int sum = 0;
    while (i) {
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

void driver() {
    memset(fenwick, 0, sizeof(fenwick));
}
