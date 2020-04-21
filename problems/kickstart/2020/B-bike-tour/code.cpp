#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;

    vector<int> v(N);
    for (int i = 0; i < N; ++i)
        cin >> v[i];

    int n = 0;
    for (int i = 1; i + 1 < N; ++i)
        if (v[i] > v[i - 1] && v[i] > v[i + 1])
            ++n;
    return n;
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
