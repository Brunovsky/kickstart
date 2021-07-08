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
    long fac = 1;
    for (int i = 2; i <= 10; i++) {
        fac *= i;
    }
    long P;
    cin >> P;
    long coins = 0;
    for (int i = 10; i >= 1; i--) {
        while (P >= fac) {
            P -= fac;
            coins++;
        }
        fac /= i;
    }
    cout << coins << '\n';
    return 0;
}
