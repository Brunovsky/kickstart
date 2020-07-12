#include <bits/stdc++.h>

using namespace std;

// *****

int N;
vector<int> fenwick;

int prefix_sum(int i) {
  int sum = 0;
  while (i) {
    sum += fenwick[i];
    i -= i & -i;
  }
  return sum;
}

void add(int i, int n) {
  while (i <= N) {
    fenwick[i] += n;
    i += i & -i;
  }
}

auto solve() {
  cin >> N >> ws;
  fenwick.assign(N + 1, 0);

  int k = 1;
  vector<int> ans(N);

  for (int i = 0; i < N; i++) {
    int citations;
    cin >> citations;
    add(citations, 1);

    if (i - k >= prefix_sum(k))
      k++;

    ans[i] = k;
  }

  return ans;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ":";
    for (int n : solution) {
      cout << ' ' << n;
    }
    cout << '\n';
  }
  return 0;
}
