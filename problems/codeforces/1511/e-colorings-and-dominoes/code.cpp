#include <bits/stdc++.h>

using namespace std;

// *****

constexpr uint64_t MAXW = 300'010, MOD = 998244353, INV2 = (MOD + 1) / 2;
uint64_t trail[MAXW], pw2[MAXW];

auto solve() {
    int N, M, W = 0;
    cin >> N >> M >> ws;
    vector<vector<bool>> white(N, vector<bool>(M));
    for (int i = 0; i < N; i++) {
        string line;
        cin >> line;
        for (int j = 0; j < M; j++) {
            white[i][j] = line[j] == 'o';
            W += white[i][j];
        }
    }

    trail[0] = trail[1] = 0, pw2[0] = 1, pw2[1] = 2;

    for (int n = 2; n <= W; n++) {
        pw2[n] = 2 * pw2[n - 1] % MOD;
    }

    uint64_t mod2w = INV2; // 2^W-2
    trail[0] = 1, trail[1] = INV2;
    for (int n = 2; n <= W; n++) {
        mod2w = 2 * mod2w % MOD;
        trail[n] = (INV2 * (INV2 + trail[n - 2])) % MOD;
    }

    uint64_t ans = 0;
    // first horizontal cells
    for (int row = 0; row < N; row++) {
        int i = 0;
        while (i < M && !white[row][i])
            i++;

        while (i < M) {
            int j = i + 1, n = 1;
            while (j < M && white[row][j]) {
                j++, n++;
                ans = (ans + trail[n - 2]) % MOD;
            }

            i = j;
            while (i < M && !white[row][i])
                i++;
        }
    }
    // now vertical cells
    for (int col = 0; col < M; col++) {
        int i = 0;
        while (i < N && !white[i][col])
            i++;

        while (i < N) {
            int j = i + 1, n = 1;
            while (j < N && white[j][col]) {
                j++, n++;
                ans = (ans + trail[n - 2]) % MOD;
            }

            i = j;
            while (i < N && !white[i][col])
                i++;
        }
    }

    return ans * mod2w % MOD;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
