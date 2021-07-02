#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        int nim = 0;
        for (int i = 1; i <= N; i++) {
            int n;
            cin >> n;
            if (i % 2 == 0) {
                nim ^= n;
            }
        }
        if (nim == 0) {
            cout << "second\n";
        } else {
            cout << "first\n";
        }
    }
    return 0;
}
