#include <bits/stdc++.h>

using namespace std;

// *****

#define MAX 1'000'000'000'000'000'000UL
#define T 10'000'000

mt19937 mt(random_device{}());
uniform_int_distribution<ulong> dist(1, MAX);

int main() {
    cout.sync_with_stdio(false);
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        ulong L = dist(mt);
        ulong R = dist(mt);
        printf("%lu %lu\n", L, R);
    }
    return 0;
}
