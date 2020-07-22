#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;

// *****

constexpr u64 MOD = 1'000'000'007UL;

u64 N, K;
vector<u64> A;

// compute n^k modulo MOD
u64 expo(u64 n, u64 k) {
  u64 b = n, power = 1;
  while (k > 0) {
    if (k & 1)
      power = (power * b) % MOD;
    k >>= 1;
    b = (b * b) % MOD;
  }
  return power;
}

// compute (n^(K + 1) - 1) / (n - 1) - 1 modulo MOD
// c = (n^(K + 1) - 1) * (n - 1)^-1 - 1 modulo MOD
u64 power(u64 n) {
  if (n == 1)
    return K;

  u64 a = expo(n, K + 1);
  u64 b = expo(n - 1, MOD - 2);
  u64 c = ((a + MOD - 1) * b + MOD - 1) % MOD;
  return c;
}

void read() {
  u64 x, y, C, D, E1, E2, F;
  cin >> N >> K >> x >> y >> C >> D >> E1 >> E2 >> F >> ws;
  A.assign(N + 1, 0);
  A[1] = (x + y) % F;
  for (u64 n = 2; n <= N; ++n) {
    u64 X = (C * x + D * y + E1) % F;
    u64 Y = (D * x + C * y + E2) % F;
    x = X;
    y = Y;
    A[n] = (x + y) % F;
  }
}

auto solve() {
  read();
  u64 weight_sum = 0, POWER_SUM = 0;

  for (u64 n = N, c = 1; n > 0; --n, ++c) {
    weight_sum = (weight_sum + c * A[n]) % MOD;
    POWER_SUM = (POWER_SUM + power(n) * weight_sum) % MOD;
  }

  return POWER_SUM;
}

// (n^1 + n^n + n^3 + ... + n^K) = (n^(K+1) - 1) / (n - 1) - 1
// Base 1: N.A1 + (N-1).A2 + ... + 1.An
// Base 2: (N-1).A2 + ... + 1.An
// Base 3: (N-2).A3 + ... + 1.An
// ...
// Base n: 1.An

// *****

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
