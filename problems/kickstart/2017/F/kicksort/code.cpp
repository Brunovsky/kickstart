#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N >> ws;
    vector<int> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }

    bool up = (N & 1) == 1;
    int minimum = 1;
    int maximum = N;

    int center_min = (N - 1) / 2;
    int center_max = (N - 1) / 2;
    int center = center_min;

    for (int i = 1; i <= N; i++) {
        if (A[center] == minimum) {
            ++minimum;
        } else if (A[center] == maximum) {
            --maximum;
        } else {
            return "NO";
        }
        up = !up;
        center = up ? ++center_max : --center_min;
    }

    return "YES";
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
