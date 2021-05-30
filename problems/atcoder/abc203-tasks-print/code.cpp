#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

auto solve() {
    int N, K;
    cin >> N >> K;
    int sum = 0;
    for (int n = 1; n <= N; n++) {
        for (int k = 1; k <= K; k++) {
            sum += 100 * n + k;
        }
    }
    return sum;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
