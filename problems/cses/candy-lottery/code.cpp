#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false);
    cout << setprecision(6) << showpoint << fixed;
    int N, K;
    cin >> N >> K;
    vector<double> upto(K + 1);
    for (int k = 1; k <= K; k++) {
        upto[k] = pow(1.0 * k / K, N);
    }
    double ans = 0;
    for (int k = 1; k <= K; k++) {
        ans += k * (upto[k] - upto[k - 1]);
    }
    cout << ans << endl;
    return 0;
}
