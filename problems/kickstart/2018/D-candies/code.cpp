#include <bits/stdc++.h>

using namespace std;

#define debug(...) fprintf(stderr, __VA_ARGS__)

// *****

int N, O;
long long D;

vector<long long> S;
vector<long long> sum_prefix;

void read() {
  long long X1, X2, A, B, C, M, L;
  cin >> N >> O >> D >> ws >> X1 >> X2 >> A >> B >> C >> M >> L;
  S.resize(N + 1, 0);
  S[1] = X1, S[2] = X2;
  for (int i = 3; i <= N; i++) {
    S[i] = (A * S[i - 1] + B * S[i - 2] + C) % M;
  }
  for (int i = 1; i <= N; i++) {
    S[i] = S[i] + L;
  }
}

auto solve() {
  read();

  sum_prefix.resize(N + 1, 0);
  for (int i = 1; i <= N; i++) {
    sum_prefix[i] = sum_prefix[i - 1] + S[i];
  }

  multiset<long long, greater<long long>> sums;
  int L = 0, R = 1; // [L,R)
  long long best = INT_MIN;
  int odds = 0;
  sums.insert(sum_prefix[0]);

  while (L <= N) {
    // move L forward one
    if (L < R) {
      sums.erase(sums.find(sum_prefix[L]));
      odds -= S[L++] & 1;
    }
    // move R as far as possible
    while (R <= N && odds + (S[R] & 1) <= O) {
      sums.insert(sum_prefix[R]);
      odds += S[R++] & 1;
    }
    // continue if interval is empty
    if (L == R) {
      L++, R++;
      continue;
    }
    auto it = sums.lower_bound(D + sum_prefix[L - 1]);
    if (it != sums.end()) {
      best = max(best, *it - sum_prefix[L - 1]);
    }
  }

  return best != INT_MIN ? to_string(best) : "IMPOSSIBLE"s;
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
