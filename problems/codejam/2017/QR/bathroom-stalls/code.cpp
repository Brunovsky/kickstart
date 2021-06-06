#include <bits/stdc++.h>

using namespace std;

// *****

// max(Ls,Rs) - min(Ls,Rs) = 0 or 1

auto solve() {
    map<long, long, greater<long>> lencnt;
    long N, K;
    cin >> N >> K;
    lencnt[N] = 1;

    while (!lencnt.empty()) {
        auto entry = *lencnt.begin();
        lencnt.erase(lencnt.begin());
        long len = entry.first, cnt = entry.second;
        long max = (len - 0) / 2;
        long min = (len - 1) / 2;
        K -= cnt;
        if (K <= 0) {
            return to_string(max) + ' ' + to_string(min);
        }
        if (max)
            lencnt[max] += cnt;
        if (min)
            lencnt[min] += cnt;
    }

    return to_string(0) + ' ' + to_string(0);
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
