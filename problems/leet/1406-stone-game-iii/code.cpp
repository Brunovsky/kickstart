#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string stoneGameIII(vector<int> &stones) {
    int N = stones.size();
    if (N == 0)
      return "Tie";

    stones.push_back(0);
    stones.push_back(0);
    stones.push_back(0);

    // score of A - score of B
    vector<int> A_first(N + 3, 0);
    vector<int> B_first(N + 3, 0);

    A_first[N - 1] = stones[N - 1];
    B_first[N - 1] = -stones[N - 1];

    for (int i = N - 2; i >= 0; --i) {
      int A1, A2, A3;
      int B1, B2, B3;

      A1 = stones[i] + B_first[i + 1];
      A2 = stones[i] + stones[i + 1] + B_first[i + 2];
      A3 = stones[i] + stones[i + 1] + stones[i + 2] + B_first[i + 3];

      B1 = -stones[i] + A_first[i + 1];
      B2 = -stones[i] - stones[i + 1] + A_first[i + 2];
      B3 = -stones[i] - stones[i + 1] - stones[i + 2] + A_first[i + 3];

      A_first[i] = max(A1, max(A2, A3));
      B_first[i] = min(B1, min(B2, B3));
    }

    if (A_first[0] == 0)
      return "Tie";
    return A_first[0] > 0 ? "Alice" : "Bob";
  }
};

// *****

int main() {
  return 0;
}
