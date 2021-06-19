#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    unordered_map<int, int> cnt;
    for (int i = 0; i < N; i++) {
        int num;
        cin >> num;
        cnt[num]++;
    }
    long ans = 0;
    for (auto [_, count] : cnt) {
        ans += 1L * count * (N - count);
    }
    cout << ans / 2 << '\n';
    return 0;
}
