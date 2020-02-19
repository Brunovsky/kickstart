#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;
using i8 = int8_t;

// *****

u64 testL, testR, N;
vector<u64> small_primes;

static bool ordered(u64 a, u64 b, u64 c) noexcept {
  return a <= b && b <= c;
}

// Find all odd primes at most N, inclusive. Start at 3.
void small_odd_sieve() {
  assert(small_primes.empty());
  small_primes.reserve(30'000);
  N |= 1; // make N odd.

  vector<i8> is_composite(N / 2, 0);

  for (u64 n = 3, i = 0; n <= N; ++i, n += 2) {
    if (is_composite[i])
      continue;
    for (u64 j = n * n; j <= N; j += 2 * n) {
      is_composite[(j - 3) / 2] = 1;
    }
  }

  for (u64 i = 0, n = 3; n <= N; ++i, n += 2) {
    if (!is_composite[i])
      small_primes.push_back(n);
  }
}

// Count all odd primes in [L, R], inclusive. Require R at most N * N.
u64 sieve(u64 L, u64 R) {
  assert(R <= N * N);

  if (R < L || R == 0)
    return 0;
  L = L | 1;
  R = (R - 1) | 1;
  if (R < L)
    return 0;

  vector<i8> is_composite((R - L + 2) / 2, 0);
  u64 count = 0;

  for (const u64 p : small_primes) {
    if (p * p > R)
      break;

    // (k-1)p < L <= kp
    // (k-1) < L/p <= k

    const u64 k = max((p + L - 1) / p | 1, p);
    u64 n = k * p;

    for (; n <= R; n += 2 * p) {
      assert(ordered(L, n, R) && n == (n | 1));
      is_composite[(n - L) >> 1] = 1;
    }
  }

  for (u64 comp : is_composite)
    count += !comp;

  return count;
}

// Count integers in the range [L, R] that are 2 modulo 4.
// There are cleaner ways of doing this
u64 count_k1(u64 L, u64 R) {
  u64 count = (R - L) / 4;
  u64 Lhi = L + (R - L) % 4;

  for (u64 n = L; n <= Lhi; ++n) {
    if ((n % 4) == 2)
      ++count;
  }

  return count;
}

// X = 2^k * SUM(pi^ei)
// divisors: (k + 1)(e1 + 1)(e2 + 1)...(en + 1) = (k + 1)M
// even divisors: kM; odd divisors: M.
// M = 1 --> k = 0,1,2,3 --> X = 1,2,4,8.
//  > X = 1 is already counted by sieve(L, R).
//  > X = 2 is already counted by count_k1(L, R).
//  > X = 4 is already counted by sieve(L / 4, R / 4).
//  > X = 8 is not already, so it is checked manually.
// M = 2 --> k = 0,1,2 --> X = p, 2p, 4p for each p.
//  > k = 0 --> X = p solutions are counted with sieve(L, R).
//  > k = 1 --> X = 2p solutions are already counted in count_k1(L, R).
//  > k = 2 --> X = 4p solutions are counted with sieve(L / 4, R / 4).
// M > 2 --> k = 1 --> X = 2p^2, 2pq, etc.
//  > Counted in count_k1(L, R).

auto solve() {
  u64 L = testL, R = testR;

  u64 A8 = ordered(L, 8, R);          // O(1)
  u64 B1 = sieve(L, R);               // O((R - L) sqrt R)
  u64 B2 = sieve((L + 3) / 4, R / 4); // O((R - L) sqrt R)
  u64 C = count_k1(L, R);             // O(1)
  u64 total = A8 + B1 + B2 + C;

  return total;
}

// *****

void reparse_test() {
  cin >> testL >> testR >> ws;
}

// *****

int main() {
  N = 31623UL; // sqrt(1'000'000'000) rounded up
  small_odd_sieve();

  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
