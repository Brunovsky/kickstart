#include <bits/stdc++.h>

using namespace std;

using u64 = uint64_t;
using i64 = int64_t;

u64 N, M;
vector<u64> A;

auto solve() {
  u64 maxA = *max_element(A.begin(), A.end());

  u64 M_power_two = 0, M_exponent_two = 0;
  if (M > 0) {
    u64 m = M;
    ++M_power_two;
    while (m >>= 1)
      (M_power_two <<= 1), ++M_exponent_two;
  }

  u64 lower_mask = (M_power_two << 1) - 1;
  u64 upper_mask = numeric_limits<u64>::max() ^ lower_mask;

  // All As must have the same upper mask
  u64 k_upper = upper_mask & A.front();

  for (u64 a : A) {
    if ((upper_mask & a) != k_upper)
      return i64(-1); // busted
  }
  if (M == 0)
    return i64(A.front());

  std::vector<u64> A_set_count(70, 0);
  u64 pow = M_power_two;
  u64 l = M_exponent_two;
  u64 min_sum = 0;
  u64 k = k_upper;

  while (pow > 0) {
    for (u64 a : A) {
      if (pow & a) {
        ++A_set_count[l];
      }
    }

    min_sum += min(A_set_count[l], N - A_set_count[l]) * pow;
    if (min_sum > M)
      return i64(-1); // busted

    if (A_set_count[l] >= N - A_set_count[l])
      k |= pow;

    pow >>= 1;
    --l;
  }

  pow = M_power_two;
  l = M_exponent_two;

  while (pow > 0) {
    if ((k & pow) == 0) {
      u64 increment = (N - 2 * A_set_count[l]) * pow;
      if (min_sum + increment <= M) {
        k |= pow;
        min_sum += increment;
      }
    }
    pow >>= 1;
    --l;
  }

  return i64(k);
}

// *****

void reparse_test() {
  cin >> N >> M;
  A.resize(N);
  for (u64 i = 0; i < N; ++i)
    cin >> A[i];
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
