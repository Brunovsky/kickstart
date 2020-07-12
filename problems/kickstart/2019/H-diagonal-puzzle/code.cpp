#include <bits/stdc++.h>

using namespace std;

// *****

#define WHITE '.'
#define BLACK '#'

int N;
vector<string> grid;

char diag(int i) {
  return grid[i / 2][(i + 1) / 2];
}

int check(int count) {
  // verify this actually works
  for (int i = 0; i < N; i++) {
    for (int j = 1; j < i; j++) {
      bool b1 = diag(i - j) == grid[i - j][0];
      bool b2 = diag(i + j) == grid[i][j];
      if (b1 != b2)
        return 4 * N; // busted
    }
  }
  for (int i = 1; i < N; i++) {
    for (int j = i + 2; j < N; j++) {
      bool b1 = diag(j - i) == grid[0][j - i];
      bool b2 = diag(i + j) == grid[i][j];
      if (b1 != b2)
        return 4 * N; // busted
    }
  }
  for (int i = 0; i < N; i++) {
    count += grid[i][i] == WHITE;
  }
  for (int i = 1; i < N; i++) {
    count += grid[i - 1][i] == WHITE;
  }
  for (int i = 1; i < N; i++) {
    count += grid[i][0] != diag(i);
  }
  for (int i = 2; i < N; i++) {
    count += grid[0][i] != diag(i);
  }
  return min(count, 4 * N - 2 - count);
}

auto solve() {
  cin >> N >> ws;
  grid.resize(N);
  for (int i = 0; i < N; i++) {
    getline(cin, grid[i]);
  }

  int keep = check(0);
  for (int i = 0; i < N; i++) {
    grid[i][i] = grid[i][i] == BLACK ? WHITE : BLACK;
  }
  int flip = check(1);
  return min(keep, flip);
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
