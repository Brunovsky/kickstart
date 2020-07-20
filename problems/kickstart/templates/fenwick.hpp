#include <bits/stdc++.h>

using namespace std;

// *****

int N;
vector<int> fenwick;

int prefix_sum(int i) {
    int sum = 0;
    while (i) {
        sum += fenwick[i];
        i -= i & -i;
    }
    return sum;
}

void add(int i, int n) {
    while (i <= N) {
        fenwick[i] += n;
        i += i & -i;
    }
}

// *****

void driver() {
    fenwick.assign(N + 1, 0);
}
