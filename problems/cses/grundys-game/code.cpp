#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false);
    constexpr int MAXN = 1500;
    vector<int> grundy(MAXN + 1, 0);
    grundy[0] = grundy[1] = grundy[2] = 0;
    for (int n = 3; n <= MAXN; n++) {
        vector<bool> seen(n + 1);
        for (int i = 1; i < n - i; i++) {
            int k = grundy[i] ^ grundy[n - i];
            seen[min(k, n)] = true;
        }
        for (int i = 0; i <= n; i++) {
            if (!seen[i]) {
                grundy[n] = i;
                break;
            }
        }
    }

    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        if (N <= MAXN && grundy[N] == 0) {
            cout << "second" << endl;
        } else {
            cout << "first" << endl;
        }
    }
    return 0;
}
