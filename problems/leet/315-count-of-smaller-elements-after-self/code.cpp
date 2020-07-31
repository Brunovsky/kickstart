#include <bits/stdc++.h>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

// *****

using int2 = pair<int, int>;
using min_tree = tree<int2, null_type, less<int2>, rb_tree_tag,
                      tree_order_statistics_node_update>;

class Solution {
public:
  vector<int> countSmaller(const vector<int> &nums) {
    int N = nums.size();
    vector<int> count(N);
    min_tree elems;
    for (int i = N - 1; i >= 0; i--) {
      count[i] = elems.order_of_key({nums[i], -1});
      elems.insert({nums[i], i});
    }
    return count;
  }
};

// *****

int main() {
  return 0;
}
