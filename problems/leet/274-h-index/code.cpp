#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int hIndex(vector<int> &citations) {
    sort(begin(citations), end(citations), greater<int>{});
    int N = citations.size();
    for (int i = 1; i <= N; i++) {
      if (citations[i - 1] < i) {
        return i - 1;
      }
    }
    return N;
  }
};

// *****

int main() {
  return 0;
}
