#include <bits/stdc++.h>

using namespace std;

// *****

int get(unsigned long k) {
    if (k & (k - 1)) {
        unsigned long n = 64 - __builtin_clzll(k);
        return 1 - get((1UL << n) - k);
    } else {
        return 0;
    }
}

auto solve() {
    unsigned long K;
    cin >> K;
    return get(K);
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
