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
    vector<array<int, 3>> intv(N);
    for (auto& [t, l, r] : intv) {
        cin >> t >> l >> r;
    }
    // YOU CAN BRUTE FORCE DUDE
    int count = 0;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            auto [ta, la, ra] = intv[i];
            auto [tb, lb, rb] = intv[j];
            int m = 10 * ta + tb;
            count += (la <= rb && lb <= ra) && (m == 11);
            count += (la < rb && lb <= ra) && (m == 12 || m == 31 || m == 32);
            count += (la <= rb && lb < ra) && (m == 13 || m == 21 || m == 23);
            count += (la < rb && lb < ra) &&
                     (m == 14 || m == 24 || m == 34 || m == 44 || m == 22 || m == 33 ||
                      m == 41 || m == 42 || m == 43);
        }
    }
    cout << count << endl;
    return 0;
}
