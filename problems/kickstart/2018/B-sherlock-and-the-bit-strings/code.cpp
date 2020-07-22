#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 102
#define MAXP 1'000'000'000'000'000'073LL
#define MAXD 15

constexpr int MASK(int n) {
  return (1 << min(n, MAXD + 1)) - 1;
}

constexpr int GLB_MASK = (1 << 16) - 1;

// 0001111110000
//    ^b1  ^b2 0-indexed
constexpr int BITS(int m, int bits) {
  return __builtin_popcount(m & MASK(bits));
}

int N, K;
long long P;
vector<int> A[MAXN];
vector<int> C[MAXN];

bool good(int m, int x) {
  int constraints = A[x].size();
  for (int i = 0; i < constraints; i++) {
    if (BITS(m, x - A[x][i] + 1) != C[x][i]) {
      return false;
    }
  }
  return true;
}

// dp[x][m]: number of strings meeting constraints with B>=x such that
// s[max(0,x-15)...x] == m
long long dp[MAXN][GLB_MASK + 1];

auto solve() {
  cin >> N >> K >> P >> ws;
  for (int i = 1; i < MAXN; i++) {
    A[i].clear();
    C[i].clear();
  }
  for (int i = 0; i < K; i++) {
    int a, b, c;
    cin >> a >> b >> c >> ws;
    A[b].push_back(a);
    C[b].push_back(c);
  }

  memset(dp, 0, sizeof(dp));
  // careful: even masks only; let the string end with the null byte
  for (int m = 0; m <= MASK(N + 1); m += 2) {
    dp[N + 1][m] = 1;
  }
  for (int x = N; x >= 1; x--) {
    for (int m = 0; m <= MASK(x); m++) {
      if (good(m, x)) {
        int m_0 = ((m << 1) | 0) & MASK(x + 1);
        int m_1 = ((m << 1) | 1) & MASK(x + 1);
        dp[x][m] = min(dp[x + 1][m_0] + dp[x + 1][m_1], MAXP);
      }
    }
  }

  string prefix;
  int m = 0;
  for (int x = 1; x <= N; x++) {
    if (dp[x][m] >= P) {
      m = ((m | 0) << 1) & MASK(x + 1);
      prefix += '0';
    } else {
      P -= dp[x][m];
      m = ((m | 1) << 1) & MASK(x + 1);
      prefix += '1';
    }
  }
  assert(P == 1);
  return prefix;
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
