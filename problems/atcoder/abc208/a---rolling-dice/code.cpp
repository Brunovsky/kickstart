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
    int A, B;
    cin >> A >> B;
    if (B >= A && B <= 6 * A) {
        cout << "Yes\n";
    } else {
        cout << "No\n";
    }
    return 0;
}
