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
    long A, B, C;
    cin >> A >> B >> C;
    long ans = 0;
    long ops = 0;
    if (A > C) {
        swap(A, C);
    }
    if (C == max(B, C)) {
        if (C - B <= B - A) {
            ans = (B - A) - (C - B);
        } else {
            if (A % 2 != C % 2)
                A++, ops++;
            ans = (A + C) / 2 - B;
        }
    } else {
        ans = B - A + B - C;
    }
    cout << ans + ops << '\n';
    return 0;
}
