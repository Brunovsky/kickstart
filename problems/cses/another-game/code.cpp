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
        int parity[2] = {};
        for (int i = 0; i < N; i++) {
            parity[x[i] % 2]++;
        }
        if (parity[1] > 0) {
            cout << "first" << endl;
        } else {
            cout << "second" << endl;
        }
    }
    return 0;
}
