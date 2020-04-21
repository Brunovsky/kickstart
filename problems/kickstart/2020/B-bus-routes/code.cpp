#include <bits/stdc++.h>

using namespace std;

// *****

using ull = unsigned long long;

auto solve() {
    int N;
    ull D;
    cin >> N >> D;

    vector<ull> X(N + 1);
    for (int i = 1; i <= N; ++i)
        cin >> X[i];

    auto is_possible = [&](ull s) {
        for (int i = 1; i <= N; ++i) {
            ull mod = s % X[i];
            if (mod != 0)
                s += X[i] - mod;
        }
        return s <= D;
    };

    ull lo = 1, hi = D;
    while (lo < hi) {
        ull mi = (lo + hi + 1) / 2;
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
