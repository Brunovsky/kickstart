#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void gameOfLife(vector<vector<int>> &board) {
    int R = int(board.size()) - 1;
    if (R < 0)
      return;
    int C = int(board[0].size()) - 1;
    for (int r = 0; r <= R; r++) {
      for (int c = 0; c <= C; c++) {
        int nw = (r > 0 && c > 0) ? board[r - 1][c - 1] & 1 : 0;
        int nc = (r > 0) ? board[r - 1][c] & 1 : 0;
        int ne = (r > 0 && c < C) ? board[r - 1][c + 1] & 1 : 0;

        int cw = (c > 0) ? board[r][c - 1] & 1 : 0;
        int ce = (c < C) ? board[r][c + 1] & 1 : 0;

        int sw = (r < R && c > 0) ? board[r + 1][c - 1] & 1 : 0;
        int sc = (r < R) ? board[r + 1][c] & 1 : 0;
        int se = (r < R && c < C) ? board[r + 1][c + 1] & 1 : 0;

        int neighbours = nw + nc + ne + cw + ce + sw + sc + se;
        bool if_live = neighbours == 2 || neighbours == 3;
        bool if_dead = neighbours == 3;
        if ((board[r][c] && if_live) || (!board[r][c] && if_dead)) {
          board[r][c] |= 2;
        }
      }
    }
    for (int r = 0; r <= R; r++) {
      for (int c = 0; c <= C; c++) {
        board[r][c] >>= 1;
      }
    }
  }
};

// *****

int main() {
  return 0;
}
