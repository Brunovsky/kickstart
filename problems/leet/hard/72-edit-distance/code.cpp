#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

class Solution {
 public:
  // levenshtein distance
  int minDistance(const string& a, const string& b) {
    if (a.size() < b.size()) return minDistance(b, a);

    size_t A = a.size(), B = b.size();

    vector<size_t> cur_row(B + 1), old_row(B + 1);

    for (size_t j = 0; j <= B; ++j) old_row[j] = j;

    for (size_t i = 1; i <= A; ++i) {
      cur_row[0] = i;

      for (size_t j = 1; j <= B; ++j) {
        size_t s = a[i - 1] == b[j - 1] ? 0 : 1;

        size_t subst = s + old_row[j - 1];
        size_t inser = 1 + old_row[j];
        size_t remov = 1 + cur_row[j - 1];

        cur_row[j] = min(subst, min(inser, remov));
      }

      swap(cur_row, old_row);
    }

    return old_row[B];
  }
};

// *****

void test() {
  Solution S;

  cout << S.minDistance("horse", "ros") << ' ' << 3 << endl;
  cout << S.minDistance("intention", "execution") << ' ' << 5 << endl;
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
