#include <bits/stdc++.h>

using namespace std;

// redefine N to be the total number of slots (2N in the original problem)
// let C(N, k) := (N choose k)
// how to place the M couples without restrictions?
// C(N, 2M) --> allows permuting couples (i.e. couples in any order) and mates
// (i.e. male and female).
//  Place the M couples    - W = C(N, 2M) total placements, Q < W with no poems
//  Permute the M pairs    - M! ways  --> already counted
//  Swap each mate         - 2^M ways --> already counted
//  Place the N-2M couples - (N-2M)! ways
//     answer: Q * (N-2M)!

// let Ai be the set of W placements that have couple 1 sitting together.
// then Q = W - |A1 U A2 U ... U AM|
// compute union with inclusion exclusion:
// |Ai| = place couple i anywhere, then place the rest:
//                       1!  2  * C(N - 1, 1) * C(N - 2, 2M - 2)
// total:     C(M, 1) * ^
// |Ai & Aj| = place couples i,j anywhere, then place the rest:
//                       2!  4  * C(N - 2, 2) * C(N - 4, 2M - 4)
// total:     C(M, 2) * ^
// ...
// |A1 & ... & Aj| = place all couples anywhere:
//                       M! 2^M * C(N - M, M) * C(N - 2M, 2M - 2M)
// total:     C(M, M) * ^

// notice:
// C(M, k)            -->  M!               k!         (M - k)!
// C(N - k, k)        -->  (N - k)!         k!         (N - 2k)!
// C(N - 2k, N - 2M)  -->  (N - 2k)!        (N - 2M)!  (2M - 2k)!

// *****

using i64 = int64_t;
constexpr i64 MOD = 1000000007;
constexpr int MAX = 200000;

i64 gcd(i64 a, i64 b, i64 &x, i64 &y) {
  i64 xn = 1, yn = 0;
  x = 0, y = 1;
  while (a != 0) {
    i64 q = b / a;
    b = b % a;
    x = x - q * xn;
    y = y - q * yn;
    swap(a, b);
    swap(x, xn);
    swap(y, yn);
  }
  return b;
}

i64 invmod(i64 a, i64 mod) {
  i64 x, y;
  gcd(a, mod, x, y);
  return x < 0 ? (mod + x % mod) : (x % mod);
}

vector<i64> fac;
vector<i64> inv;
vector<i64> po2;

void prepare() {
  fac.resize(MAX + 1);
  inv.resize(MAX + 1);
  po2.resize(MAX + 1);
  fac[0] = fac[1] = inv[0] = inv[1] = 1;
  po2[0] = 1;
  po2[1] = 2;
  for (i64 n = 2; n <= MAX; n++) {
    fac[n] = n * fac[n - 1] % MOD;
    inv[n] = invmod(fac[n], MOD);
    po2[n] = 2 * po2[n - 1] % MOD;
  }
}

inline i64 compute(int N, int M, int k) {
  i64 num = (fac[M] * fac[N - k] % MOD) * po2[k] % MOD;
  i64 den = (inv[k] * inv[M - k] % MOD) * inv[N - 2 * M] % MOD;
  return num * den % MOD;
}

inline i64 total(int N, int M) {
  return fac[N] * inv[N - 2 * M] % MOD;
}

auto solve() {
  int N, M;
  cin >> N >> M >> ws;
  N *= 2; // adjust N

  i64 Q = total(N, M);
  i64 i = -1;
  for (int k = 1; k <= M; k++) {
    Q = (Q + i * compute(N, M, k) + MOD) % MOD;
    i = -i;
  }
  return Q * fac[N - 2 * M] % MOD;
}

// *****

int main() {
  prepare();
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
