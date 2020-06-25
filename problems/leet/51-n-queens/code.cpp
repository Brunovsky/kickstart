#include <bits/stdc++.h>

using namespace std;

// *****

using board_t = vector<string>;
static constexpr char QUEEN = 'Q', FREE = ' ', ATTACKED = '.';

board_t make_board(int n) {
  return board_t(n, string(n, FREE));
}

void print_board(const board_t &board, int n, int r) {
  printf("Processing row %d...\n", r);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      putchar(board[i][j]);
    }
    putchar('\n');
  }
  putchar('\n');
}

vector<board_t> solve(const board_t &parent_board, int n, int r) {
  if (r == n)
    return {parent_board};

  vector<board_t> solutions;

  for (int c = 0; c < n; ++c) {
    if (parent_board[r][c] == ATTACKED)
      continue;

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

    auto boards = solve(child_board, n, r + 1);
    for (auto board : boards)
      solutions.push_back(move(board));
  }
  return solutions;
}

class Solution {
public:
  vector<board_t> solveNQueens(int n) {
    const auto tmpl = make_board(n);
    return solve(tmpl, n, 0);
  }
};

// *****

int main() {
  const vector<vector<string>> expected_4 = {
      {
          ".Q..",
          "...Q",
          "Q...",
          "..Q.",
      },
      {
          "..Q.",
          "Q...",
          "...Q",
          ".Q..",
      },
  };

  auto reversed_4 = expected_4;
  reverse(reversed_4.begin(), reversed_4.end());
  const auto actual_4 = Solution{}.solveNQueens(4);
  if (actual_4 != expected_4 && actual_4 != reversed_4) {
    printf("Error: Actual did not match solution.\n");
    printf("Expected solutions: %lu\n", expected_4.size());
    printf("Actual   solutions: %lu\n", actual_4.size());
  }

  return 0;
}
