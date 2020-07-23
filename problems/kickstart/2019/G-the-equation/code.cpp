#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    ulong N, M;
    cin >> N >> M;
    vector<ulong> A(N);
    for (ulong i = 0; i < N; ++i) {
        cin >> A[i];
    }

    ulong M_power_two = 0, M_exponent_two = 0;
    if (M > 0) {
        ulong m = M;
        ++M_power_two;
        while (m >>= 1) {
            M_power_two <<= 1;
            M_exponent_two++;
        }
    }

    ulong lower_mask = (M_power_two << 1) - 1;
    ulong upper_mask = numeric_limits<ulong>::max() ^ lower_mask;

    // All As must have the same upper mask
    ulong k_upper = upper_mask & A[0];

    for (ulong a : A) {
        if ((upper_mask & a) != k_upper) {
            return -1L; // busted
        }
    }
    if (M == 0) {
        return long(A[0]);
    }

    vector<ulong> A_set_count(70, 0);
    ulong pow = M_power_two;
    ulong l = M_exponent_two;
    ulong min_sum = 0;
    ulong k = k_upper;

    while (pow > 0) {
        for (ulong a : A) {
            if (pow & a) {
                ++A_set_count[l];
            }
        }

        min_sum += min(A_set_count[l], N - A_set_count[l]) * pow;
        if (min_sum > M) {
            return -1L; // busted
        }

        if (A_set_count[l] >= N - A_set_count[l]) {
            k |= pow;
        }

        pow >>= 1;
        --l;
    }

    pow = M_power_two;
    l = M_exponent_two;

    while (pow > 0) {
        if ((k & pow) == 0) {
            ulong increment = (N - 2 * A_set_count[l]) * pow;
            if (min_sum + increment <= M) {
                k |= pow;
                min_sum += increment;
            }
        }
        pow >>= 1;
        --l;
    }

    return long(k);
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
