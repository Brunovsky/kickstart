#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> plusOne(vector<int> &digits) {
    int i = digits.size() - 1;
    while (i >= 0 && digits[i] == 9)
      digits[i--] = 0;
    if (i >= 0) {
      ++digits[i];
    } else {
      digits[0] = 1;
      digits.push_back(0);
    }
    return digits;
  }
};

// *****

int main() {
  return 0;
}
