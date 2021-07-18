#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, M;
    cin >> N >> M;
    vector<int> A(M), C(M);
    for (int i = 0; i < M; i++) {
        cin >> A[i] >> C[i];
    }
    int G = N;
    for (int i = 0; i < M && G > 1; i++) {
        G = gcd(G, A[i]);
    }
    if (G > 1) {
        cout << -1 << '\n';
        return 0;
    }
    // can't do M^2 but can do M log M
    // greedy? pick the cheapest one then keep reducing
    vector<int> index(M);
    iota(begin(index), end(index), 0);
    sort(begin(index), end(index), [&](int i, int j) { return C[i] < C[j]; });

    G = N;
    long cost = 0;
    for (int i = 0; i < M && G > 1; i++) {
        int g = gcd(G, A[index[i]]);
        if (g < G) {
            int k = G - g;
            cost += 1LL * k * C[index[i]];
            G = g;
        }
    }
    cout << cost << '\n';
    return 0;
}
