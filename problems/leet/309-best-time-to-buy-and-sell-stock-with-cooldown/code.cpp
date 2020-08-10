#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maxProfit(const vector<int> &prices) {
    int n = prices.size();
    int buy = INT_MIN;
    int sell = 0;
    int cool = 0;
    for (int i = 0; i < n; i++) {
      int new_buy = max(buy, cool - prices[i]);
      int new_sell = buy + prices[i];
      int new_cool = max(cool, sell);
      buy = new_buy, sell = new_sell, cool = new_cool;
    }
    return max(sell, cool);
  }
};

// *****

int main() {
  cout << Solution{}.maxProfit({1, 2, 3, 0, 2}) << endl;
  cout << Solution{}.maxProfit({1, 2, 3, 0, 2, 4}) << endl;
  cout << Solution{}.maxProfit({}) << endl;
  cout << Solution{}.maxProfit({4, 2, 1}) << endl;
  cout << Solution{}.maxProfit({6, 5, 4, 3, 2, 1}) << endl;
  cout << Solution{}.maxProfit({1, 2, 3, 4, 5, 6}) << endl;
  return 0;
}
