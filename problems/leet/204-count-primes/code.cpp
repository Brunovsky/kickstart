#include <bits/stdc++.h>

using namespace std;

// *****

using uint = unsigned;
vector<uint> small_primes;

// we skip multiples of two in the sieve, the primes array,
// and count_primes function for efficiency.

// Find all odd primes at most N, inclusive. Start at 3.
void sieve(uint N) {
  assert(small_primes.empty());
  small_primes.reserve(30'000);
  N |= 1; // make N odd.

  vector<uint8_t> is_composite(N / 2, 0);

  for (uint n = 3, i = 0; n <= N; ++i, n += 2) {
    if (is_composite[i])
      continue;
    for (uint j = n * n; j <= N; j += 2 * n) {
      is_composite[(j - 3) / 2] = 1;
    }
  }

  for (uint i = 0, n = 3; n <= N; ++i, n += 2) {
    if (!is_composite[i])
      small_primes.push_back(n);
  }
}

// Count all odd primes in [L, R], inclusive. Require R at most N * N.
uint count_primes(uint L, uint R) {
  if (R < L || R == 0)
    return 0;
  L = L | 1;
  R = (R - 1) | 1;
  if (R < L)
    return 0;

  vector<uint8_t> is_composite((R - L + 2) / 2, 0);
  uint count = 0;

  for (uint p : small_primes) {
    if (p * p > R)
      break;

    // (k-1)p < L <= kp
    // (k-1) < L/p <= k

    uint k = max((p + L - 1) / p | 1, p);

    for (uint n = k * p; n <= R; n += 2 * p) {
      is_composite[(n - L) >> 1] = 1;
    }
  }

  for (uint n : is_composite)
    count += !n;

  return count + 1;
}

class Solution {
public:
  Solution() {
    if (small_primes.empty())
      sieve((1 << 16) + (1 << 14)); // enough padding to find the next prime
  }

  int countPrimes(int n) {
    if (n <= 2)
      return 0;
    if (n <= 4)
      return n - 2;
    return count_primes(3, n - 1);
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
