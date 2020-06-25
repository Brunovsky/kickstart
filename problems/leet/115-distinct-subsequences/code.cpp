#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int numDistinct(const string &s, const string &t) {
    int S = s.size();
    int T = t.size();
    if (!S || !T)
      return 0;

    // There may be problems with size_t being too small for this dp.
    vector<size_t> prev(T + 1, 0);
    vector<size_t> next(T + 1, 0);
    prev[T] = next[T] = 1;

    for (int i = S - 1; i >= 0; --i) {
      for (int j = T - 1; j >= 0; --j) {
        next[j] = prev[j];
        if (s[i] == t[j])
          next[j] += prev[j + 1];
      }

      swap(prev, next);
    }

    return prev[0];
  }
};

// *****

int main() {
  return 0;
}
