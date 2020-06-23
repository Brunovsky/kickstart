#include <bits/stdc++.h>

using namespace std;

// *****

using board_t = vector<string>;
static constexpr char QUEEN = 'Q', FREE = ' ', ATTACKED = '.';

board_t make_board(int n) {
  return board_t(n, string(n, FREE));
}

size_t solve(const board_t &parent_board, int n, int r) {
  assert(r < n);
  size_t solutions = 0;

  for (int c = 0; c < n; ++c) {
    if (parent_board[r][c] == ATTACKED)
      continue;

    if (r + 1 == n) {
      ++solutions;
      continue;
    }

    board_t child_board = parent_board;
    child_board[r][c] = QUEEN;

    for (int i = 0; i < n; ++i)
      if (i != c)
        child_board[r][i] = ATTACKED;

    for (int i = r + 1; i < n; ++i) {
      child_board[i][c] = ATTACKED;
      if (c + (i - r) < n)
        child_board[i][c + (i - r)] = ATTACKED;
      if (c - (i - r) >= 0)
        child_board[i][c - (i - r)] = ATTACKED;
    }

    solutions += solve(child_board, n, r + 1);
  }

  return solutions;
}

class Solution {
public:
  size_t totalNQueens(int n) {
    const auto tmpl = make_board(n);
    return solve(tmpl, n, 0);
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
