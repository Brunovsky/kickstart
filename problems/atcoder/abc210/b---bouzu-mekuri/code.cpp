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
    string s;
    cin >> N >> s;
    for (int i = 0; i < N; i++) {
        if (s[i] == '1') {
            if (i & 1) {
                cout << "Aoki\n";
            } else {
                cout << "Takahashi\n";
            }
            break;
        }
    }
    return 0;
}
