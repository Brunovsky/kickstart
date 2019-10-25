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

u32 N, K;
vector<u64> V;

auto solve() {
  u64 sum = 0;
  for (u64 v : V) sum += v;

  if (K == 0) {
    double mean = double(sum) / double(N);
    return mean;
  }
}

// *****

void prep() {}

void parse_test() {
  cin >> N >> K;
  V.resize(N);
  for (u32 i = 0; i < N; ++i) cin >> V[i];
  sort(V.begin(), V.end());
}

void reset_test() {
  V.clear();
}

void print_test(u64 casenum, double solution) {
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
