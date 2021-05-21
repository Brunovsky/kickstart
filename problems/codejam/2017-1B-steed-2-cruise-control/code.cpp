#include <bits/stdc++.h>

using namespace std;

// *****

struct Horse {
    int K, S;
};

auto solve() {
    int D, N;
    cin >> D >> N;
    vector<Horse> horses(N);
    for (auto& horse : horses)
        cin >> horse.K >> horse.S;
    sort(begin(horses), end(horses), [](Horse a, Horse b) { return a.K < b.K; });

    double W = 1.0 * (D - horses[N - 1].K) / horses[N - 1].S;
    for (int i = N - 2; i >= 0; i--) {
        W = max(W, 1.0 * (D - horses[i].K) / horses[i].S);
    }
    return D / W;
}

// *****

int main() {
    cout << fixed << setprecision(6);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
