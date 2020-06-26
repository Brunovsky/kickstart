#include <bits/stdc++.h>

using namespace std;

// *****

using i64 = int64_t;

constexpr i64 MOD = 1000000007;

auto solve() {
    i64 N;
    cin >> N;
    vector<i64> nums(N);
    for (i64 i = 0; i < N; ++i) {
        cin >> nums[i];
    }

    sort(nums.begin(), nums.end());

    i64 sum = 0, power = 1;
    for (i64 i = 0, j = N - 1; i < N; ++i, --j) {
        sum += nums[i] * power;
        sum -= nums[j] * power;
        sum = sum % MOD;
        power = (2 * power) % MOD;
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
