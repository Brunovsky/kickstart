#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;

// *****

constexpr u32 MAX_HEIGHT = 1010;  // problem was here

u32 N, K;
vector<u32> A;
vector<u32> R;  // region lengths
vector<u32> P;  // backtrack pair map
vector<u32> S;  // region lengths partial sums

void fill_aux() {
  R.clear();
  P.clear();
  S.clear();

  vector<u32> H;

  H.push_back(0);
  R.push_back(0);
  S.push_back(0);

  u32 cur = A[0], len = 1;
  for (u32 i = 1; i < N; ++i, ++len) {
    if (A[i] != cur) {
      H.push_back(cur);
      R.push_back(len);
      S.push_back(S.back() + len);
      cur = A[i];
      len = 0;
    }
  }

  H.push_back(cur);
  R.push_back(len);
  S.push_back(S.back() + len);

  P.resize(H.size(), 0);

  vector<u32> visit(MAX_HEIGHT, 0);

  for (u32 i = 1; i < H.size(); ++i) {
    P[i] = visit[H[i]];
    visit[H[i]] = i;
  }
}

auto solve() {
  fill_aux();

  u32 NR = R.size();
  using cell = array<u32, 2>;
  vector<vector<cell>> dyn(NR, vector<cell>(K + 1, {0, 0}));

  for (u32 k = 0; k <= K; ++k) {
    dyn[0][k] = {0, 0};
  }

  for (u32 i = 1; i < NR; ++i) {
    u32 valley = S[i] - S[P[i]] - R[i];
    assert(valley <= N);

    dyn[i][0][1] = dyn[P[i]][0][1] + valley;
    dyn[i][0][0] = min(dyn[i][0][1], dyn[i - 1][0][0] + R[i]);

    for (u32 k = 1; k <= K; ++k) {
      dyn[i][k][1] = min(dyn[i - 1][k - 1][0], dyn[P[i]][k][1] + valley);
      dyn[i][k][0] = min(dyn[i][k][1], dyn[i - 1][k][0] + R[i]);
    }
  }

  return min(dyn[NR - 1][K][0], dyn[NR - 1][K][1]);
}

// *****

void reparse_test() {
  cin >> N >> K;
  A.assign(N, 0);
  for (u32 i = 0; i < N; ++i) cin >> A[i];
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
