#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;

// *****

u32 N, P;
vector<u32> S;

auto solve() {
  cin >> N >> P >> ws;
  S.assign(N, 0);
  for (u32 i = 0; i < N; ++i)
    cin >> S[i] >> ws;
  sort(S.begin(), S.end());

  u32 l = 0, r = P - 1;
  u32 training = 0, min_training = 0;
  for (u32 i = 0; i < P; ++i)
    training += S[r] - S[i];

  min_training = training;

  ++l, ++r;
  while (r < N) {
    // l . . . . . . r
    //   l . . . . . . r

    u32 increment = (P - 1) * (S[r] - S[r - 1]);
    training = training + increment - (S[r - 1] - S[l - 1]);
    if (training < min_training)
      min_training = training;
    ++l, ++r;
  }

  return min_training;
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
