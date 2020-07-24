#include <bits/stdc++.h>

using namespace std;

// *****

#define min3(x, y, z) min(min(x, y), z)

#define MAXLEN 61

int A, B;
string a, b;
int dp[MAXLEN][MAXLEN];

auto solve() {
    cin >> a >> b;
    A = a.size();
    B = b.size();
    memset(dp, 0, sizeof(dp));

    for (int i = 0; i <= A; i++) {
        for (int j = 0; j <= B; j++) {
            if (min(i, j) == 0) {
                dp[i][j] = max(i, j);
            } else {
                int x = dp[i - 1][j] + 1;
                int y = dp[i][j - 1] + 1;
                int z = dp[i - 1][j - 1] + (a[i - 1] != b[j - 1]);
                dp[i][j] = min3(x, y, z);
            }
        }
    }

    string ans = a;
    int i = A, j = B;
    int D = dp[A][B] / 2;
    // increase the distance of a to ans from 0 to D

    while (D && i && j) {
        int x = dp[i - 1][j] + 1;
        int y = dp[i][j - 1] + 1;
        int z = dp[i - 1][j - 1] + 1;
        if (a[i - 1] == b[j - 1]) {
            i--, j--;
        } else if (dp[i][j] == x) {
            i--, D--;
            ans.erase(i, 1);
        } else if (dp[i][j] == y) {
            j--, D--;
            ans.insert(i, 1, b[j]);
        } else if (dp[i][j] == z) {
            i--, j--, D--;
            ans[i] = b[j];
        }
    }

    if (D && i) {
        ans.erase(0, D);
    } else if (D && j) {
        ans.insert(0, b.substr(0, D));
    }
    return ans;
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
