#include <bits/stdc++.h>

using namespace std;

// *****

long geo(long b, long k, long N, bool &overflow) {
    long bound = N / b;
    long last = b;
    long sum = b + 1;
    for (long i = 2; i < k; ++i) {
        if (last > bound) {
            overflow = true;
            return 0;
        }
        last *= b;
        sum += last;
    }
    overflow = sum > N;
    return sum;
}

auto solve() {
    long N;
    cin >> N;
    long k = 0, e = 1;
    while (e < N) {
        ++k, e <<= 1;
    }
    while (k > 2) {
        long l = 2, r = N - 2;
        bool overflow;
        while (l <= r) {
            long b = (l + r) / 2;
            long res = geo(b, k, N, overflow);
            if (overflow) {
                r = b - 1;
            } else if (res == N) {
                return b;
            } else {
                l = b + 1;
            }
        }
        --k;
    }
    return N - 1;
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
