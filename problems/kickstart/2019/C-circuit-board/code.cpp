#include <bits/stdc++.h>

using namespace std;

// *****

int R, C, K;
vector<vector<int>> V;
vector<vector<int>> range;

inline bool out_of_bounds(const multiset<int> &span, int num) {
  if (span.empty())
    return false;
  return num - *span.begin() > K || *span.rbegin() - num > K;
}

void rangify(int r) {
  int i = 0, j = 0;

  multiset<int> span;
  span.insert(V[r][i]);

  while (++j < C) {
    while (out_of_bounds(span, V[r][j])) {
      range[r][i] = span.size();
      span.erase(span.find(V[r][i++]));
    }
    span.insert(V[r][j]);
  }

  while (i < C) {
    range[r][i] = span.size();
    span.erase(span.find(V[r][i++]));
  }

  assert(span.empty());
}

int subcircuit(int r, int c) {
  int min_range = range[r][c];
  int rows = 1;
  int area = min_range;

  while (++r < R) {
    min_range = min(min_range, range[r][c]);
    area = max(area, min_range * ++rows);
  }

  return area;
}

auto solve() {
  cin >> R >> C >> K >> ws;
  V.assign(R, vector<int>(C, 0));
  range.assign(R, vector<int>(C, 0));
  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      cin >> V[r][c] >> ws;
    }
  }

  for (int r = 0; r < R; ++r) {
    rangify(r);
  }

  int area = 1;
  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      area = max(area, subcircuit(r, c));
    }
  }
  // ^ this could be O(RC) instead of O(R^2 C), but I'm lazy

  return area;
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
