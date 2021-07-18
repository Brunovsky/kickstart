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
    int N, A, X, Y;
    cin >> N >> A >> X >> Y;
    if (N <= A) {
        cout << N * X << '\n';
    } else {
        int E = N - A;
        cout << A * X + E * Y << '\n';
    }
    return 0;
}
