#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int D;
    string C;
    cin >> D >> C;
    int L = C.size();
    int cnt = 0;
    int dmg = 0;
    for (int i = 0; i < L; i++) {
        if (C[i] == 'S') {
            dmg += 1 << (i - cnt);
            cnt++;
        }
    }
    if (cnt > D) {
        return "IMPOSSIBLE"s;
    }
    int swaps = 0;
    while (dmg > D) {
        int i = C.rfind("CS");
        int power = count(begin(C), begin(C) + i, 'C');
        swap(C[i], C[i + 1]);
        swaps++;
        dmg -= 1 << power;
    }
    return to_string(swaps);
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
