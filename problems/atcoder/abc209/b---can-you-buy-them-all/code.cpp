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
    int N, X;
    cin >> N >> X;
    vector<int> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    long need = 0;
    for (int i = 0; i < N; i++) {
        need += A[i];
        if (i & 1) {
            need--;
        }
    }
    if (need <= X) {
        cout << "Yes\n";
    } else {
        cout << "No\n";
    }
    return 0;
}
