#include <bits/stdc++.h>

using namespace std;

// *****

using i64 = int64_t;

i64 geo(i64 b, i64 k, i64 N, bool &overflow) {
    i64 bound = N / b;
    i64 last = b;
    i64 sum = b + 1;
    for (i64 i = 2; i < k; ++i) {
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
    i64 N;
    cin >> N;
    i64 k = 0, e = 1;
    while (e < N) {
        ++k, e <<= 1;
    }
    while (k > 2) {
        i64 l = 2, r = N - 2;
        bool overflow;
        while (l <= r) {
            i64 b = (l + r) / 2;
            i64 res = geo(b, k, N, overflow);
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
