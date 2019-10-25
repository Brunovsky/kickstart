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

u64 N;

u64 solve() {
  if (N < 10) return (N % 2) == 1;

  u64 m = 0, pow10 = 1;

  while (N > 10 * pow10) {
    pow10 *= 10;
    ++m;
  }

  u64 U = 0;
  u64 B = N;

  while (pow10 > 0) {
    u64 prevU = U;
    U = B / pow10;
    B = B % pow10;

    if (m == 0) return (U % 2) == 1;

    if (U % 2 == 1) {
      u64 up = pow10 - B;
      u64 down = B + 1 + (pow10 - 1) / 9UL;
      return U == 9 ? down : min(up, down);
    }

    pow10 /= 10;
    --m;
  }

  return 0UL;
}

// *****

void parse_test() {
  cin >> N >> ws;
}

void reset_test() {}

void print_test(u64 casenum, u64 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
