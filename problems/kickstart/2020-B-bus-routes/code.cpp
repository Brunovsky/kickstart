#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 1003

int N;
ulong D;
ulong X[MAXN];

bool is_possible(ulong s) {
    for (int i = 1; i <= N; ++i) {
        ulong mod = s % X[i];
        if (mod != 0) {
            s += X[i] - mod;
        }
    }
    return s <= D;
}

auto solve() {
    cin >> N >> D;

    memset(X, 0, sizeof(X));
    for (int i = 1; i <= N; ++i) {
        cin >> X[i];
    }

    ulong lo = 1, hi = D;
    while (lo < hi) {
        ulong mi = (lo + hi + 1) / 2;
        if (is_possible(mi)) {
            lo = mi;
        } else {
            hi = mi - 1;
        }
    }

    return hi;
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
