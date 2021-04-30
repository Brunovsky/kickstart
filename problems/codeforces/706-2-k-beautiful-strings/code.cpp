#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    string s;
    cin >> N >> K >> ws >> s;
    array<int, 26> cnt{};
    int required = 0;
    if (N % K != 0) {
        return "-1"s;
    }
    if (K == 1 || s == string('z', N)) {
        return s;
    }

    for (int i = 0; i < N && required <= K; i++) {
        int n = s[i] - 'a';
        cnt[n]++;
        required += cnt[n] % K == 1;
    }
    assert(required >= K);
    if (required == K) {
        return s;
    }

    array<int, 26> total = cnt;

    int i = N - 1;
    while (i > nz && required > K) {
        int n = s[i] - 'a';
        cnt[n]--;
        required -= cnt[n] % K == 0;
        i--;
    }

    int i = 0;
    while (i < N) {
        int n = s[i] - 'a';
        if (cnt[n] % K == 0 && required == K) {
            break;
        }
        cnt[n]++;
        required += cnt[n] % K == 1;
        i++;
    }
    string ans = s.substr(0, i);
    for (int n = 0; n < 26; n++) {
        while (cnt[n] % K != 0) {
            ans.push_back('a' + n), cnt[n]++;
        }
    }
    assert(int(ans.size()) == N);
    return ans;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << solve() << endl;
    }
    return 0;
}
