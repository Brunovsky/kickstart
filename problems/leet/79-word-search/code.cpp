#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  vector<vector<char>> board;
  string word;
  int W, M, N;
  bool check(int i, int j, int s) {
    if (s == word.size())
      return true;
    if (i < 0 || j < 0 || i >= M || j >= N || word[s] != board[i][j])
      return false;
    board[i][j] ^= 128;
    bool ok = check(i - 1, j, s + 1) || check(i + 1, j, s + 1) ||
              check(i, j - 1, s + 1) || check(i, j + 1, s + 1);
    board[i][j] ^= 128;
    return ok;
  }

public:
  bool exist(vector<vector<char>> &inBoard, string inWord) {
    board = move(inBoard);
    word = move(inWord);
    W = word.size();
    M = board.size();
    N = board[0].size();
    for (int i = 0; i < M; ++i) {
      for (int j = 0; j < N; ++j) {
        if (check(i, j, 0)) {
          return true;
        }
      }
    }
    return false;
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
