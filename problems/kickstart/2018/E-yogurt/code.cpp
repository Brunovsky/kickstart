#include <bits/stdc++.h>

using namespace std;

// *****

int N, K;
vector<int> A;

auto solve() {
  cin >> N >> K >> ws;
  A.resize(N);
  for (int i = 0; i < N; i++) {
    cin >> A[i];
  }
  sort(A.begin(), A.end());

  int day = 0;
  int y = 0;
  int eaten = 0;

  while (y < N) {
    while (y < N && A[y] <= day)
      ++y;
    int count = 0;
    while (y < N && count < K && A[y] > day)
      ++y, ++eaten, ++count;
    ++day;
  }

  return eaten;
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
