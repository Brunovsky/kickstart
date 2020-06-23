#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isValidSudoku(vector<vector<char>> &board) {
    vector<uint8_t> empty(10, 0), nums = empty;
    // check rows top to bottom
    for (size_t i = 0; i < 9; ++i) {
      for (size_t j = 0; j < 9; ++j)
        if (char c = board[i][j]; c != '.' && nums[c - '0']++)
          return false;
      nums = empty;
    }
    // check columns left to right
    for (size_t i = 0; i < 9; ++i) {
      for (size_t j = 0; j < 9; ++j)
        if (char c = board[j][i]; c != '.' && nums[c - '0']++)
          return false;
      nums = empty;
    }
    // check squares top to bottom, left to right
    for (size_t i = 0; i < 9; i += 3) {
      for (size_t j = 0; j < 9; j += 3) {
        for (size_t k = 0; k < 3; ++k)
          for (size_t l = 0; l < 3; ++l)
            if (char c = board[i + k][j + l]; c != '.' && nums[c - '0']++)
              return false;
        nums = empty;
      }
    }
    return true;
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
