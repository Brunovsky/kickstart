#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int K, C, S;
    cin >> K >> C >> S;
    int G = (K + C - 1) / C;
    if (G > S) {
        return "IMPOSSIBLE"s;
    }

    vector<unsigned long> powk(C, 1);
    for (int i = 1; i < C; i++) {
        powk[i] = powk[i - 1] * K;
    }

    vector<unsigned long> idx(G, 1);
    for (int i = 0; i < K; i++) {
        idx[i / C] += i * powk[i % C];
    }

    stringstream ss;
    for (int i = 0; i < G; i++) {
        ss << (i ? " " : "") << idx[i];
    }
    return ss.str();
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
