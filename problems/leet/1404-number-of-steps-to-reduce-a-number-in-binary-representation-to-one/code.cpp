#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int numSteps(string s) {
    size_t S = s.size() - 1;
    size_t i = s.rfind('1');
    int cnt = S - i;
    while (i > 0) {
      size_t j = s.rfind('0', i - 1);
      if (j == string::npos) {
        cnt += i + 2;
        break;
      }
      cnt += i - j + 1;
      i = j;
    }
    return cnt;
  }
};

// *****

int main() {
  Solution sol;
  cout << sol.numSteps("1"s) << endl;
  cout << sol.numSteps("10"s) << endl;
  cout << sol.numSteps("100"s) << endl;
  cout << sol.numSteps("1000"s) << endl;
  cout << sol.numSteps("100010110"s) << endl;
  cout << sol.numSteps("1101"s) << endl;
  cout << sol.numSteps("11111"s) << endl;
  return 0;
}
