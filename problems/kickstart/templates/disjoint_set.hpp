#include <bits/stdc++.h>

using namespace std;

/**
 * Static union find / disjoint set
 */

// *****

namespace disjoint_set {

int N;
vector<int> next, rank;

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

// *****

void driver() {
    next.resize(N);
    rank.assign(N, 0);
    iota(next.begin(), next.end(), 0);

    // .....
}

} // namespace disjoint_set
