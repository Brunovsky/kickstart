#include <bits/stdc++.h>

using namespace std;

// *****

struct Tower {
    int id, len = 0;
};

constexpr int E = 10; // expected
int N = 20, B = 15;

double pow10[18];
double dp[20][20][11];
vector<Tower> towers;

void dp_two_towers() {
    pow10[0] = 1;
    for (int i = 1; i < 18; i++) {
        pow10[i] = 10 * pow10[i - 1];
    }
    for (int a = B - 1; a >= 0; a--) {
        for (int b = a; b >= 0; b--) {
            double sum = 0;
            for (int x = 0; x <= 9; x++) {
                dp[a][b][x] = max(dp[a + 1][b][E] + x * pow10[a],
                                  dp[a][b + 1][E] + x * pow10[b]);
                dp[b][a][x] = dp[a][b][x];
                sum += dp[a][b][x];
            }
            dp[a][b][E] = dp[b][a][E] = sum / 10.0;
        }
    }
}

int best(int i, int j, int x) {
    int a = towers[i].len, b = towers[j].len;
    if (dp[a + 1][b][E] + x * pow10[a] < dp[a][b + 1][E] + x * pow10[b])
        return j;
    else
        return i;
}

auto solve() {
    towers.assign(N, {});
    for (int i = 0; i < N; i++) {
        towers[i].id = i + 1;
    }

    for (int xchg = 0, d; xchg < N * B; xchg++) {
        cin >> d;
        assert(0 <= d && d <= 9);
        int x = 0;
        for (int y = 1, M = towers.size(); y < M; y++) {
            x = best(x, y, d);
        }
        cout << towers[x].id << endl;
        towers[x].len++;
        if (towers[x].len == B) {
            towers.erase(begin(towers) + x);
        }
    }

    assert(towers.empty());
}

// *****

int main() {
    cout.setf(ios::unitbuf);
    unsigned T;
    unsigned long P;
    cin >> T >> N >> B >> P;
    dp_two_towers();
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    int ans;
    cin >> ans;
    cerr << "judge said: " << ans << endl;
    return 0;
}
