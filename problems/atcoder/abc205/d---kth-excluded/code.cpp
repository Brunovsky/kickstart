#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;
    vector<long> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    sort(begin(A), end(A));
    A.erase(unique(begin(A), end(A)), end(A));
    auto less_than = [&](long x) { return lower_bound(begin(A), end(A), x) - begin(A); };
    for (int q = 0; q < Q; q++) {
        long Kth;
        cin >> Kth;
        long L = 0, R = long(1e18) + N + 1;
        while (L + 1 < R) {
            long M = (L + R) / 2;
            int lo = less_than(M);
            long Mth = M - lo;
            if (Mth <= Kth) {
                L = M;
            } else {
                R = M;
            }
        }
        cout << L << endl;
    }
    return 0;
}
