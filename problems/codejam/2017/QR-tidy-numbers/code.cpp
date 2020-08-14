#include <bits/stdc++.h>

using namespace std;

// *****

long tab10[19];

void setup() {
    tab10[0] = 1;
    for (int i = 1; i < 19; i++)
        tab10[i] = 10 * tab10[i - 1];
}

auto solve() {
    long N;
    cin >> N;
    long upper = N / 10;
    long lower = N % 10;
    long lo = lower;
    int i = 1;
    while (upper) {
        int hi = upper % 10;
        if (hi > lo) {
            lower = tab10[i] - 1;
            upper--;
            hi = upper % 10;
        }
        lo = hi;
        lower = lower + lo * tab10[i++];
        upper = upper / 10;
    }
    return lower;
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
