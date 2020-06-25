#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> grayCode(int n) {
    vector<int> gray;
    gray.reserve(1 << n);
    int value = 0, mask = 0;
    while (mask++ < (1 << n)) {
      gray.push_back(value);
      int flip = 1, bit = mask;
      while (!(bit & 1))
        flip <<= 1, bit >>= 1;
      value = value ^ flip;
    }
    return gray;
  }
};

// *****

int main() {
  return 0;
}
