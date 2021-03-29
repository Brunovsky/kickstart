#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long B, N;
    cin >> B >> N, N--;
    vector<long> M(B);
    for (int b = 0; b < B; b++)
        cin >> M[b];

    long lo = 0, hi = 10'000'000'000'000;
    while (lo <= hi) {
        long time = (lo + hi) / 2;
        long attended = 0;
        for (long each : M)
            attended += (time + each - 1) / each;
        if (attended <= N) {
            lo = time + 1;
        } else {
            hi = time - 1;
        }
    }
    long time = lo - 1;
    for (long each : M)
        N -= (time + each - 1) / each;
    for (int b = 0; b < B; b++)
        if (time % M[b] == 0) {
            if (N-- == 0)
                return b + 1;
        }

    return -1;
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
