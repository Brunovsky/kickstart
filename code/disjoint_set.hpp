#include <bits/stdc++.h>

using namespace std;

// *****

struct disjoint_set {
    int N;
    vector<int> next, rank;

    disjoint_set(int N) : N(N), next(N), rank(N, 0) {
        iota(begin(next), end(next), 0);
    }

    int find(int i) {
        while (i != next[i]) {
            i = next[i] = next[next[i]];
        }
        return i;
    }

    void join(int i, int j) {
        i = find(i);
        j = find(j);
        if (i != j) {
            if (rank[i] < rank[j]) {
                swap(i, j);
            }
            next[j] = i;
            rank[i] += rank[i] == rank[j];
        }
    }
};
