#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
  int N;
  cin >> N >> ws;
  vector<int> A(3 * N); // good guy
  vector<int> B(3 * N); // bad guy
  for (int i = 0; i < 3 * N; i++) {
    cin >> A[i];
  }
  for (int i = 0; i < 3 * N; i++) {
    cin >> B[i];
  }
  sort(A.begin(), A.end(), greater<int>{});
  return 0;
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
