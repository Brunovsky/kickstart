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
    long A, B, C, D;
    cin >> A >> B >> C >> D;
    if (C * D <= B) {
        cout << -1 << endl;
        return 0;
    }
    long X = 0;
    int ops = 0;
    while (X * D < A) {
        X += C;
        A += B;
        ops++;
    }
    cout << ops << endl;
    return 0;
}
