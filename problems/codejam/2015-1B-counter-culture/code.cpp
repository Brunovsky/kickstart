#include <bits/stdc++.h>

using namespace std;

// *****

#define rev(S) reverse(begin(S), end(S))

long pow10[17];

void setup() {
    pow10[0] = 1;
    for (int i = 1; i < 17; i++)
        pow10[i] = 10 * pow10[i - 1];
}

auto solve() {
    long ops, n, s;
    string N, S;

    cin >> N, n = stol(N);
    if (n <= 10)
        return n;

    int D = N.size();
    ops = 10, S = "10", s = 10;

    auto add = [&](long k) { s += k, ops += k, S = to_string(s); };
    auto flip = [&]() { rev(S), s = stol(S), ops++; };

    // make it to 10^(D-1) first
    for (int d = 2; d < D; d++) {
        add(pow10[d / 2] - 1);
        flip();
        add(pow10[d - d / 2] - 1);
        assert(s == pow10[d]);
    }
    if (n == s)
        return ops;

    long ops_save = ops + n - s; // just increment

    if (stol(N.substr(0, D / 2)) == pow10[D / 2 - 1])
        return min(ops_save, ops + stol(N.substr(D / 2)));

    if (stol(N.substr(D / 2)) == 0)
        ops++, n--, N = to_string(n);

    rev(N), n = stol(N);
    add(stol(N.substr(D - D / 2)));
    flip(), rev(N), n = stol(N);
    add(stol(N.substr(D / 2)) - 1);

    return min(ops_save, ops);
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
