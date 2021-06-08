#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false);
    cout << setprecision(6) << showpoint << fixed;
    int N;
    cin >> N;
    vector<int> r(N);
    for (int i = 0; i < N; i++) {
        cin >> r[i];
    }
    double ans = 0;
    for (int a = 0; a < N; a++) {
        for (int b = a + 1; b < N; b++) {
            if (r[a] <= r[b]) {
                ans += 0.5 * (r[a] - 1) / r[b];
            } else {
                ans += 1 - 0.5 * (r[b] + 1) / r[a];
            }
        }
    }
    cout << ans << endl;
    return 0;
}
