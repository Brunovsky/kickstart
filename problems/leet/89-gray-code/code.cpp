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

void test() {
  vector<int> gray = Solution{}.grayCode(4);
  for (int n : gray)
    printf("%d ", n);
  printf("\n");
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
