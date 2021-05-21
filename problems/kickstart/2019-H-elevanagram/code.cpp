#include <bits/stdc++.h>

using namespace std;

// *****

// Let Ei, Oi be the number of 'i's in even and odd positions. Then:
// SUM i(Ei - Oi) = 0 (mod 11)  <==>
// SUM i(2Ei - Ai) = 0 (mod 11)  <==>
// SUM iEi = 6 SUM iAi (mod 11)

// dp[i][s][k]: it is possible to find an assignment for E1,...,Ei such that:
// SUM Ei = s   &&   SUM iEi = k (mod 11)

bool dp[10][500][11];
int A[10];

auto solve() {
    memset(A, 0, sizeof(A));
    memset(dp, 0, sizeof(dp));

    for (int i = 1; i <= 9; i++) {
        cin >> A[i];
        if (A[i] > 50) {
            A[i] = 50 + (A[i] & 1);
        }
    }

    int Asum = 0;
    int iAi = 0;
    dp[0][0][0] = true;
    for (int i = 1; i <= 9; i++) {
        for (int Ei = 0; Ei <= A[i]; Ei++) {
            for (int p = 0; p <= Asum; p++) {
                for (int c = 0; c <= 10; c++) {
                    dp[i][Ei + p][(c + i * Ei) % 11] |= dp[i - 1][p][c];
                }
            }
        }
        Asum += A[i];
        iAi += i * A[i];
    }
    return dp[9][(Asum + 1) / 2][(6 * iAi) % 11] ? "YES" : "NO";
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
