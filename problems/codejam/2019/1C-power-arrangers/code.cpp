#include <bits/stdc++.h>

using namespace std;

// *****

int cnt[5];
char seen[120];
vector<int> idx;

auto clear(int full) {
    return [full](int i) {
        return cnt[seen[i] - 'A'] == full;
    };
}

void filter(int full) {
    idx.erase(remove_if(begin(idx), end(idx), clear(full)), end(idx));
}

void solve() {
    idx.resize(119);
    iota(begin(idx), end(idx), 0);
    string ans(5, ' ');
    constexpr int fac[] = {24, 6, 2};
    for (int j = 0; j < 3; j++) {
        memset(cnt, 0, sizeof(cnt));
        for (int i : idx) {
            cout << 5 * i + j + 1 << endl;
            cin >> seen[i];
            cnt[seen[i] - 'A']++;
        }
        filter(fac[j]);
        ans[j] = seen[idx[0]];
    }
    cout << 5 * idx[0] + 5 << endl;
    cin >> ans[3];
    ans[4] = 'A' + "ABCDE"s.find_first_not_of(ans);
    cout << ans << endl;
    char verdict;
    cin >> verdict;
    assert(verdict == 'Y');
}

// *****

int main() {
    unsigned T, F;
    cin >> T >> F;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
