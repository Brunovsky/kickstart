#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false);
    int N, K;
    cin >> N >> K;
    vector<int> p(K);
    for (int i = 0; i < K; i++) {
        cin >> p[i];
    }

    string ans;
    vector<int> grundy(N + 1);
    for (int n = 1; n <= N; n++) {
        vector<bool> seen(K + 1);
        for (int i : p) {
            if (i <= n) {
                seen[min(grundy[n - i], K)] = true;
            }
        }
        for (int i = 0; i <= K; i++) {
            if (!seen[i]) {
                grundy[n] = i;
                break;
            }
        }
        ans += grundy[n] == 0 ? 'L' : 'W';
    }

    cout << ans << endl;
    return 0;
}
