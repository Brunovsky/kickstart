#include <bits/stdc++.h>

using namespace std;

// how many palindromic strings with length <= N?
// L + L + L^2 + L^2 + ...
// N = 2k   --> SUM = 2(L + L^2 + ... + L^k)
// N = 2k+1 --> SUM = 2(L + L^2 + ... + L^k) + L^(k+1)

// *****

#define MAXK 100'000'000'000'073L

long pown(long n, long k) {
  long q = 1;
  while (k--) {
    if (q >= MAXK) {
      return MAXK;
    }
    q *= n;
  }
  return q;
}

bool is_palindromic_prefix(const string &s, long M) {
  long S = s.length();
  assert(M >= S);
  long i = M - S;
  long j = S - 1;
  while (i < j) {
    if (s[i++] != s[j--]) {
      return false;
    }
  }
  return true;
}

long construct(long L, long N, long K) {
  // b.....b < c....c
  string prefix;
  long S = 0;
  for (S = 1; S <= N; S++) {
    prefix += '_';
    for (long q = 0; q < L; q++) {
      prefix[S - 1] = 'a' + q;
      long palindromies = 0;
      for (long M = S; M <= N; M++) {
        if (is_palindromic_prefix(prefix, M)) {
          long choices = max(0L, (M + 1) / 2 - S);
          palindromies += pown(L, choices);
          if (palindromies >= K) {
            goto select;
          }
        }
      }
      K -= palindromies;
    }
    return 0; // we didn't bust K, so K is too large
  select:
    K -= is_palindromic_prefix(prefix, S);
    if (K == 0) {
      return S;
    }
  }
  return 0;
}

auto solve() {
  long L, N, K;
  cin >> L >> N >> K >> ws;
  if (L == 1 || K <= N) {
    return K <= N ? K : 0;
  }
  assert(K <= MAXK);
  // N and A same parity
  long A = min(N, 200 + (N & 1));
  long Nn = N - A; // I'm tired :(
  long len = construct(L, A, K - Nn);
  return len ? Nn + len : 0;
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
