#include <bits/stdc++.h>

using namespace std;

// *****

mt19937 mt(random_device{}());
uniform_int_distribution<int> dist(1, 1000000000);
uniform_int_distribution<int> distN(50, 1000);

#define T 100

int main() {
    printf("%d\n", T);
    for (int t = 1; t <= T; t++) {
        long C = dist(mt);
        long J = dist(mt);
        int N = distN(mt);
        printf("%d\n", N);
        using pll = pair<long, long>;
        vector<pll> nums(N);
        for (int i = 0; i < N; i++) {
            nums[i] = {dist(mt), dist(mt)};
        }
        sort(begin(nums), end(nums), [C, J](pll lhs, pll rhs) {
            return C * lhs.first + J * lhs.second < C * rhs.first + J * rhs.second;
        });
        for (int i = 0; i < N; i++) {
            printf("%ld %ld\n", nums[i].first, nums[i].second);
        }
    }
    return 0;
}
