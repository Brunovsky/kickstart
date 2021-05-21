#include <bits/stdc++.h>

using namespace std;

// *****

#define MASK 127UL

void solve() {
    ulong R1, R2, R3, R4, R5, R6;
    ulong C;
    cout << 220 << endl;
    cin >> C;
    R4 = (C >> 55) & MASK;
    R5 = (C >> 44) & MASK;
    R6 = (C >> 36) & MASK;
    cout << 54 << endl;
    cin >> C;
    C -= (1 << 13) * R4 + (1 << 10) * R5 + (1 << 9) * R6;
    R1 = (C >> 54) & MASK;
    R2 = (C >> 27) & MASK;
    R3 = (C >> 18) & MASK;
    char line[73];
    sprintf(line, "%lu %lu %lu %lu %lu %lu", R1, R2, R3, R4, R5, R6);
    cout << line << endl;
    int ans;
    cin >> ans;
    assert(ans == 1);
}

// *****

int main() {
    unsigned T, W;
    cin >> T >> W;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
