#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    cin >> N >> K;
    vector<int> nums(N);
    for (int i = 0; i < N; i++) {
        cin >> nums[i];
    }
    sort(begin(nums), end(nums));
    nums.erase(unique(begin(nums), end(nums)), end(nums));
    N = nums.size();
    vector<int> lengths;
    for (int i = 1; i < N; i++) {
        int diff = nums[i] - nums[i - 1];
        lengths.push_back(diff - 1);
    }
    sort(rbegin(lengths), rend(lengths));
    int L = lengths.size();
    int lo = nums[0] - 1, hi = K - nums[N - 1];

    // both outside
    int ans = lo + hi;

    // both inside and different
    if (L >= 2) {
        ans = max(ans, (lengths[0] + 1) / 2 + (lengths[1] + 1) / 2);
    }
    // both inside and same
    if (L >= 1) {
        ans = max(ans, lengths[0]);
    }
    // one inside one outside
    if (L >= 1) {
        ans = max(ans, (lengths[0] + 1) / 2 + max(lo, hi));
    }
    return 1.0 * ans / K;
}

// *****

int main() {
    unsigned T;
    cout << setprecision(7) << showpoint << fixed;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
