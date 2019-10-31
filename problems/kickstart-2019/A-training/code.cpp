#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

u32 N, P;
vector<u32> S;

auto solve() {
  sort(S.begin(), S.end());

  u32 l = 0, r = P - 1;
  u32 training = 0, min_training = 0;
  for (u32 i = 0; i < P; ++i) training += S[r] - S[i];

  min_training = training;

  ++l, ++r;
  while (r < N) {
    // l . . . . . . r
    //   l . . . . . . r

    u32 increment = (P - 1) * (S[r] - S[r - 1]);
    training = training + increment - (S[r - 1] - S[l - 1]);
    if (training < min_training) min_training = training;
    ++l, ++r;
  }

  return min_training;
}

// *****

void prep() {}

void parse_test() {
  cin >> N >> P >> ws;
  S.resize(N);
  for (u32 i = 0; i < N; ++i) cin >> S[i] >> ws;
}

void reset_test() {
  S.clear();
}

void print_test(u64 casenum, u32 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  prep();
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
