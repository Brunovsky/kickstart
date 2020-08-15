#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;

    unordered_map<int, int> cnt;
    for (int i = 0; i < N * (2 * N - 1); i++) {
        int n;
        cin >> n;
        cnt[n]++;
    }

    vector<int> nums;
    for (auto entry : cnt) {
        if (entry.second & 1) {
            nums.push_back(entry.first);
        }
    }
    sort(begin(nums), end(nums));

    for (int n : nums)
        cout << ' ' << n;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
        cout << '\n';
    }
    return 0;
}
