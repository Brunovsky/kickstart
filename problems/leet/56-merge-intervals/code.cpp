#include <bits/stdc++.h>

using namespace std;

// *****

using pair_t = vector<int>;

class Solution {
public:
  vector<pair_t> merge(vector<pair_t> &intv) {
    if (intv.empty())
      return intv;

    sort(intv.begin(), intv.end());

    size_t l = 0;
    for (size_t r = 1; r < intv.size(); ++r) {
      if (intv[l][1] >= intv[r][0]) {
        intv[l][1] = max(intv[l][1], intv[r][1]);
      } else {
        intv[++l] = intv[r];
      }
    }

    intv.resize(l + 1);
    return intv;
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
