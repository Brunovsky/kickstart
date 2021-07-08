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
    int N;
    long K;
    cin >> N >> K;
    vector<int> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    vector<int> idx(N);
    iota(begin(idx), end(idx), 0);
    sort(begin(idx), end(idx), [&](int a, int b) { return A[a] < A[b]; });
    long each = K / N;
    int rem = K - N * each;
    vector<long> ans(N, each);
    for (int i = 0; i < rem; i++) {
        ans[idx[i]]++;
    }
    for (int i = 0; i < N; i++) {
        cout << ans[i] << '\n';
    }
    return 0;
}
