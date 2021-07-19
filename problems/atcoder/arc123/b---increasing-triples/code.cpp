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
    cin >> N;
    vector<int> A(N), B(N), C(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> B[i];
    }
    for (int i = 0; i < N; i++) {
        cin >> C[i];
    }
    sort(begin(A), end(A));
    sort(begin(B), end(B));
    sort(begin(C), end(C));

    multiset<int> a(begin(A), end(A));
    multiset<int> b(begin(B), end(B));
    multiset<int> c(begin(C), end(C));

    int ans = 0;

    while (!a.empty()) {
        auto ait = a.begin();
        int n = *ait;
        auto bit = b.upper_bound(n);
        if (bit == b.end()) {
            break;
        }
        int m = *bit;
        auto cit = c.upper_bound(m);
        if (cit == c.end()) {
            break;
        }

        a.erase(ait);
        b.erase(bit);
        c.erase(cit);
        ans++;
    }

    cout << ans << '\n';
    return 0;
}
