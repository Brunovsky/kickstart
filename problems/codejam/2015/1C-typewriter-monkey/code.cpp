#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int K, W, S;
    string kbd, word;
    cin >> K >> W >> S >> kbd >> word;
    int g = W;
    for (int i = 1; i < W; i++) {
        if (word.substr(i) == word.substr(0, W - i)) {
            g = i;
            break;
        }
    }
    int max_cnt = 0;
    while (W + max_cnt * g <= S)
        max_cnt++;

    int cnt[26] = {};
    for (char key : kbd)
        cnt[key - 'A']++;
    for (char w : word)
        if (cnt[w - 'A'] == 0)
            return 0.0;

    double p = 1.0;
    for (char w : word)
        p *= 1.0 * cnt[w - 'A'] / K;

    return max_cnt - p * (S - W + 1);
}

// *****

int main() {
    cout << setprecision(7) << fixed << showpoint;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
