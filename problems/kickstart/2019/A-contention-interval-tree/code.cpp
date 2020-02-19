#include <bits/stdc++.h>

using namespace std;

struct Interval {
  int L, R;

  bool contains(Interval interval) const noexcept {
    return L <= interval.L && R >= interval.R;
  }

  friend bool intersect(Interval i1, Interval i2) noexcept {
    return i1.R > i2.L && i2.R > i1.L;
  }
};

struct Node {
  int center;
  vector<Interval> intervals_left, intervals_right;
};

vector<Node> tree;

// *****

auto solve() {
  return 0;
}

// *****

void reparse_test() {}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
