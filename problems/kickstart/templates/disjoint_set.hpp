#include <bits/stdc++.h>

using namespace std;

// *****

int N;
vector<int> snext, srank;

int find(int i) {
    while (i != snext[i]) {
        i = snext[i] = snext[snext[i]];
    }
    return i;
}

void join(int i, int j) {
    i = find(i);
    j = find(j);
    if (i != j) {
        if (srank[i] < srank[j]) {
            swap(i, j);
        }
        snext[j] = i;
        srank[i] += srank[i] == srank[j];
    }
}

// *****

void driver() {
    snext.resize(N);
    srank.assign(N, 0);
    iota(snext.begin(), snext.end(), 0);
}
