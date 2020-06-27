#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> least(N + 1, 0);
    iota(least.begin(), least.end(), 0);

    for (int i = 2, n = 4; n <= N; ++i, n = i * i) {
        for (int j = n; j <= N; ++j) {
            least[j] = min(least[j], least[j - n] + 1);
        }
    }

    return least[N];
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
