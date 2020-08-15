#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long N;
    cin >> N;
    if (N == 0)
        return "INSOMNIA"s;
    bitset<10> digits;
    long k = 0;
    do {
        k += N;
        long n = k;
        while (n) {
            digits.set(n % 10);
            n /= 10;
        }
    } while (digits.count() < 10);
    return to_string(k);
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
