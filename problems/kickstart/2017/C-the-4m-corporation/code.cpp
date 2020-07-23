#include <bits/stdc++.h>

using namespace std;

// *****

#define IMPOSSIBLE INT_MAX

int MIN;
int MAX;
int MEAN;
int MED;

auto optimize(int t) {
    int a = MIN + MED;
    int b = MAX + MED;
    int c = MIN + MAX + t * MED;
    int k = t + 2;

    int min1 = INT_MIN, min2 = INT_MIN;
    int max1 = INT_MAX, max2 = INT_MAX;
    int term;
    int coef;

    // (a - 2MEAN)S <= kMEAN - c
    term = k * MEAN - c;
    coef = a - 2 * MEAN;
    if ((coef >= 0 && term < 0) || (coef > 0 && term <= 0)) {
        return IMPOSSIBLE;
    }
    if (coef > 0 && term > 0) {
        max1 = floor(1.0 * term / coef);
    }
    if (coef < 0 && term < 0) {
        min1 = ceil(1.0 * term / coef);
    }

    // (2MEAN - b)S <= c - kMEAN
    term = c - k * MEAN;
    coef = 2 * MEAN - b;
    if ((coef >= 0 && term < 0) || (coef > 0 && term <= 0)) {
        return IMPOSSIBLE;
    }
    if (coef > 0 && term > 0) {
        max2 = floor(1.0 * term / coef);
    }
    if (coef < 0 && term < 0) {
        min2 = ceil(1.0 * term / coef);
    }

    int maxS = min(max1, max2);
    int minS = max(max(min1, min2), 0);
    if (maxS >= 0 && maxS < minS) {
        return IMPOSSIBLE;
    }

    return 2 * minS + k;
}

auto solve() {
    cin >> MIN >> MAX >> MEAN >> MED >> ws;
    bool min_max = MIN <= MAX;
    bool median = MIN <= MED && MED <= MAX;
    bool mean = MIN <= MEAN && MEAN <= MAX;
    if (!(min_max && median && mean)) {
        return "IMPOSSIBLE"s;
    }
    if (MIN == MAX) {
        return "1"s;
    }
    if (MED == MEAN && MIN + MAX == 2 * MEAN) {
        return "2"s;
    }
    if (MIN + MED + MAX == 3 * MEAN) {
        return "3"s;
    }
    // need >3 departments

    auto odd = optimize(1);
    auto even = optimize(2);
    if (odd == IMPOSSIBLE && even == IMPOSSIBLE) {
        return "IMPOSSIBLE"s;
    }
    return to_string(min(odd, even));
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
