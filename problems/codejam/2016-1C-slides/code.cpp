#include <bits/stdc++.h>

using namespace std;

// *****

// Maximum ways for B is 1 << (B - 2).

#define MAXB 51

auto solve() {
    int B;
    ulong M;
    cin >> B >> M;
    if ((1UL << (B - 2)) < M) {
        cout << "IMPOSSIBLE\n";
        return;
    }

    int mat[MAXB][MAXB] = {};
    int lastv = 0;
    for (int b = 0, v = B - 1; v >= 2; b++, v--) {
        if ((1UL << b) & M)
            mat[1][v] = 1, lastv = v;
    }
    for (int u = lastv; u < B; u++) {
        for (int v = u + 1; v <= B; v++) {
            mat[u][v] = 1;
        }
    }

    cout << "POSSIBLE\n";
    for (int u = 1; u <= B; u++) {
        for (int v = 1; v <= B; v++) {
            cout << mat[u][v];
        }
        cout << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
