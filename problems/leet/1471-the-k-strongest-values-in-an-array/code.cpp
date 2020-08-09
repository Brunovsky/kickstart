#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> getStrongest(vector<int> &arr, int k) {
    int n = arr.size();
    nth_element(begin(arr), begin(arr) + (n - 1) / 2, end(arr));
    int m = arr[(n - 1) / 2];
    nth_element(begin(arr), begin(arr) + k, end(arr), [&](int i, int j) {
      int ai = abs(i - m), aj = abs(j - m);
      return ai > aj || (ai == aj && i > j);
    });
    vector<int> ans(begin(arr), begin(arr) + k);
    return ans;
  }
};

// *****

int main() {
  return 0;
}
