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
    int N, K;
    cin >> N >> K;
    vector<int> c(N);
    for (int i = 0; i < N; i++) {
        cin >> c[i];
    }
    unordered_map<int, int> freq;
    freq.reserve(K);
    int distinct = 0;
    for (int i = 0; i < K; i++) {
        distinct += freq[c[i]]++ == 0;
    }
    int ans = distinct;
    for (int i = K; i < N; i++) {
        distinct -= --freq[c[i - K]] == 0;
        distinct += freq[c[i]]++ == 0;
        ans = max(ans, distinct);
    }
    cout << ans << '\n';
    return 0;
}
