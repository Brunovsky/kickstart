#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long MOD = 1'000'000'007;

auto solve() {
    long N;
    cin >> N;
    vector<long> nums(N);
    for (long i = 0; i < N; ++i) {
        cin >> nums[i];
    }

    sort(begin(nums), end(nums));

    long sum = 0, pow2 = 1;
    for (long i = 0, j = N - 1; i < N; ++i, --j) {
        sum = (sum + (nums[i] - nums[j]) * pow2) % MOD;
        pow2 = (2 * pow2) % MOD;
    }

    return sum;
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
