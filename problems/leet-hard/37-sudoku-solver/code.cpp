#include <bits/stdc++.h>

using namespace std;

// *****

struct State {
  vector<vector<uint16_t>> board;
  bool set[9][9]{};
  bool row[9][10]{};
  bool col[9][10]{};
  bool sqr[3][3][10]{};
  uint16_t free[9][9]{};
  size_t count = 0;
};

constexpr inline uint16_t lowest_set_bit(uint16_t set) {
  return __builtin_ctz(set);
}

constexpr inline uint16_t count_set_bits(uint16_t set) {
  return __builtin_popcount(set);
}

bool assign(State &state,
            vector<tuple<uint16_t, uint16_t, uint16_t>> cascaded) {
  while (!cascaded.empty()) {
    auto [r, c, n] = cascaded.back();
    cascaded.pop_back();

    state.board[r][c] = n;
    state.set[r][c] = true;
    state.row[r][n] = true;
    state.col[c][n] = true;
    state.sqr[r / 3][c / 3][n] = true;
    if (++state.count == 81)
      return true;

    for (uint16_t i = 0; i < 9; ++i) {
      if (!state.set[r][i]) {
        uint16_t k = state.free[r][i] & ~(1 << n);
        if (k != state.free[r][i]) {
          if (k == 0)
            return false;
          state.free[r][i] = k;
          if ((k & (k - 1)) == 0) {
            cascaded.push_back({r, i, lowest_set_bit(k)});
          }
        }
      }
    }

    for (uint16_t i = 0; i < 9; ++i) {
      if (!state.set[i][c]) {
        uint16_t k = state.free[i][c] & ~(1 << n);
        if (k != state.free[i][c]) {
          if (k == 0)
            return false;
          state.free[i][c] = k;
          if ((k & (k - 1)) == 0) {
            cascaded.push_back({i, c, lowest_set_bit(k)});
          }
        }
      }
    }

    for (uint16_t i = 0, rs = (r / 3) * 3; i < 3; ++i) {
      for (uint16_t j = 0, cs = (c / 3) * 3; j < 3; ++j) {
        if (!state.set[rs + i][cs + j]) {
          uint16_t k = state.free[rs + i][cs + j] & ~(1 << n);
          if (k != state.free[rs + i][cs + j]) {
            if (k == 0)
              return false;
            state.free[rs + i][cs + j] = k;
            if ((k & (k - 1)) == 0) {
              cascaded.push_back({rs + i, cs + j, lowest_set_bit(k)});
            }
          }
        }
      }
    }
  }

  return true;
}

class Solution {
  bool dive(State &state) {
    uint16_t best_r = 0, best_c = 0, best_set_bits = 10;

    for (uint16_t r = 0; r < 9; ++r) {
      for (uint16_t c = 0; c < 9; ++c) {
        if (state.board[r][c] == 0) {
          uint16_t set_bits = count_set_bits(state.free[r][c]);
          if (set_bits < best_set_bits) {
            best_r = r;
            best_c = c;
            best_set_bits = set_bits;
            if (set_bits == 2)
              goto assign_dive;
          }
        }
      }
    }

    if (best_set_bits == 10)
      return true;

  assign_dive:

    while (state.free[best_r][best_c] > 0) {
      uint16_t n = lowest_set_bit(state.free[best_r][best_c]);
      state.free[best_r][best_c] &= ~(1 << n);

      State new_state = state;

      if (assign(new_state, {{best_r, best_c, n}})) {
        if (state.count == 81 || dive(new_state)) {
          state = std::move(new_state);
          return true;
        }
      }
    }

    return false;
  }

public:
  vector<vector<uint16_t>>
  into_u16(const vector<vector<char>> &char_board) const {
    size_t rlen = char_board.size(), clen = char_board[0].size();
    vector<vector<uint16_t>> u16_board(rlen);

    for (size_t r = 0; r < rlen; ++r) {
      u16_board[r].resize(clen);
      for (size_t c = 0; c < clen; ++c) {
        u16_board[r][c] =
            char_board[r][c] == '.' ? 0 : (char_board[r][c] - '0');
      }
    }

    return u16_board;
  }

  vector<vector<char>>
  into_char(const vector<vector<uint16_t>> &u16_board) const {
    size_t rlen = u16_board.size(), clen = u16_board[0].size();
    vector<vector<char>> char_board(rlen);

    for (size_t r = 0; r < rlen; ++r) {
      char_board[r].resize(clen);
      for (size_t c = 0; c < clen; ++c) {
        char_board[r][c] = '0' + u16_board[r][c];
      }
    }

    return char_board;
  }

  void solveSudoku(vector<vector<char>> &board) {
    State state;
    state.board = into_u16(board);
    state.count = 0;

    for (uint16_t r = 0; r < 9; ++r) {
      for (uint16_t c = 0; c < 9; ++c) {
        uint16_t n = state.board[r][c];
        if (n > 0) {
          state.set[r][c] = true;
          state.row[r][n] = true;
          state.col[c][n] = true;
          state.sqr[r / 3][c / 3][n] = true;
          state.free[r][c] = (1 << n);
          ++state.count;
        }
      }
    }

    vector<tuple<uint16_t, uint16_t, uint16_t>> cascaded;

    for (uint16_t r = 0; r < 9; ++r) {
      for (uint16_t c = 0; c < 9; ++c) {
        if (state.board[r][c] == 0) {
          for (uint16_t n = 1; n <= 9; ++n) {
            if (!state.row[r][n] && !state.col[c][n] &&
                !state.sqr[r / 3][c / 3][n]) {
              state.free[r][c] |= (1 << n);
            }
          }
          assert(state.free[r][c] != 0);
          if (count_set_bits(state.free[r][c]) == 1) {
            cascaded.push_back({r, c, lowest_set_bit(state.free[r][c])});
          }
        }
      }
    }

    assign(state, cascaded);
    if (state.count < 81)
      dive(state);
    board = into_char(state.board);
  }
};

// *****

void test() {
  vector<vector<char>> board = {
      {'.', '.', '9', '7', '4', '8', '.', '.', '.'},
      {'7', '.', '.', '.', '.', '.', '.', '.', '.'},
      {'.', '2', '.', '1', '.', '9', '.', '.', '.'},

      {'.', '.', '7', '.', '.', '.', '2', '4', '.'},
      {'.', '6', '4', '.', '1', '.', '5', '9', '.'},
      {'.', '9', '8', '.', '.', '.', '3', '.', '.'},

      {'.', '.', '.', '8', '.', '3', '.', '2', '.'},
      {'.', '.', '.', '.', '.', '.', '.', '.', '6'},
      {'.', '.', '.', '2', '7', '5', '9', '.', '.'},
  };

  Solution S;

  S.solveSudoku(board);

  for (uint16_t r = 0; r < 9; ++r) {
    for (uint16_t c = 0; c < 9; ++c) {
      cout << board[r][c] << ' ';
    }
    cout << endl;
  }
}

// *****

int main() {
  test();
  return 0;
}
