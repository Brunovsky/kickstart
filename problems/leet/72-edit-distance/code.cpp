#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  // levenshtein distance
  int minDistance(const string &a, const string &b) {
    if (a.size() < b.size())
      return minDistance(b, a);

    size_t A = a.size(), B = b.size();

    vector<size_t> cur_row(B + 1), old_row(B + 1);

    for (size_t j = 0; j <= B; ++j)
      old_row[j] = j;

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

int main() {
  return 0;
}
