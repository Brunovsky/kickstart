#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

#define max3(a, b, c) max(max(a, b), c)

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int A, B, C;
    cin >> A >> B >> C;
    cout << max3(A + B, B + C, C + A) << endl;
    return 0;
}
