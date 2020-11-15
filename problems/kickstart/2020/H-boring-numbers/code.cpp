#include <bits/stdc++.h>

using namespace std;

// *****

unsigned long ten[20], five[20], cum[20];

void setup() {
    ten[0] = five[0] = 1;
    for (int i = 1; i < 20; i++) {
        ten[i] = ten[i - 1] * 10;
        five[i] = five[i - 1] * 5;
        cum[i] = cum[i - 1] + five[i];
    }
}

long count(unsigned long N) {
    long cnt = 0;
    int k = to_string(N).length();
    int d = N / ten[k - 1];
    N = N - d * ten[k - 1];
    assert(d >= 0 && d < 10);

    // fewer than k digits
    cnt += cum[k - 1];

    // k digits but leading digit less than d
    cnt += d / 2 * five[k - 1];

    // k digits with leading d
    bool ok = (d % 2) == 1;
    for (int i = 1; ok && i < k; i++) {
        d = N / ten[k - 1 - i];
        assert(d >= 0 && d < 10);

        if (i & 1) {
            cnt += (d + 1) / 2 * five[k - 1 - i];
            ok = (d % 2) == 0;
        } else {
            cnt += d / 2 * five[k - 1 - i];
            ok = (d % 2) == 1;
        }

        N = N - d * ten[k - 1 - i];
    }

    return cnt + ok;
}

auto solve() {
    long L, R;
    cin >> L >> R;
    return count(R) - count(L - 1);
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
