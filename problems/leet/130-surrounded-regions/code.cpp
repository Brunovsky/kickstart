#include <bits/stdc++.h>

using namespace std;

// *****

const vector<array<int, 2>> dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

class Solution {
public:
  void solve(vector<vector<char>> &board) {
    if (board.empty())
      return;
    int M = board.size();
    int N = board[0].size();

    vector<array<int, 2>> frontier;

    for (int i = 0; i < M; ++i) {
      if (board[i][0] == 'O')
        frontier.push_back({i, 0});
      if (board[i][N - 1] == 'O')
        frontier.push_back({i, N - 1});
    }
    for (int j = 0; j < N; ++j) {
      if (board[0][j] == 'O')
        frontier.push_back({0, j});
      if (board[M - 1][j] == 'O')
        frontier.push_back({M - 1, j});
    }
    while (!frontier.empty()) {
      auto pp = frontier.back();
      frontier.pop_back();

      int i = pp[0], j = pp[1];
      board[i][j] = 'A';

      for (auto dir : dirs) {
        int di = i + dir[0], dj = j + dir[1];
        if (di < 0 || di >= M || dj < 0 || dj >= N || board[di][dj] != 'O')
          continue;
        frontier.push_back({di, dj});
      }
    }
    for (int i = 0; i < M; ++i) {
      for (int j = 0; j < N; ++j) {
        if (board[i][j] != 'X')
          board[i][j] = board[i][j] == 'O' ? 'X' : 'O';
      }
    }
  }
};

// *****

int main() {
  return 0;
}
