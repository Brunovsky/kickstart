#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false);
    int T;
    cin >> T;
    while (T--) {
        int N;
        cin >> N;
        vector<int> x(N);
        for (int i = 0; i < N; i++) {
            cin >> x[i];
        }
        int nim = 0;
        for (int i = 0; i < N; i++) {
            nim = nim ^ (x[i] % 4);
        }
        if (nim == 0) {
            cout << "second" << endl;
        } else {
            cout << "first" << endl;
        }
    }
    return 0;
}
