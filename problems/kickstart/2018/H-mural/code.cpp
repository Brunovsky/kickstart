#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
  int N;
  cin >> N >> ws;
  string mural;
  getline(cin, mural);

  int curr = 0;
  int mid = (N + 1) / 2;
  for (int i = 0; i < (N + 1) / 2; i++) {
    curr += mural[i] - '0';
  }

  int maximum = curr;
  for (int i = 0, j = mid; j < N; i++, j++) {
    curr = curr + mural[j] - mural[i];
    maximum = max(maximum, curr);
  }
  return maximum;
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
