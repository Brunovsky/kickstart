#include <bits/stdc++.h>

using namespace std;

// *****

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right)
      : val(x), left(left), right(right) {}
};

class Solution {
  vector<TreeNode *> generateTreeRange(int min, int max) const {
    assert(max - min >= -1);
    if (max - min == -1) {
      return {nullptr};
    } else if (max - min == 0) {
      return {new TreeNode(min)};
    } else if (max - min == 1) {
      return {new TreeNode(min, nullptr, new TreeNode(max)),
              new TreeNode(max, new TreeNode(min), nullptr)};
    } else {
      vector<TreeNode *> list;
      for (int root = min; root <= max; ++root) {
        vector<TreeNode *> leftList = generateTreeRange(min, root - 1);
        vector<TreeNode *> rightList = generateTreeRange(root + 1, max);
        for (TreeNode *left : leftList) {
          for (TreeNode *right : rightList) {
            list.push_back(new TreeNode(root, left, right));
          }
        }
      }
      return list;
    }
  }

public:
  vector<TreeNode *> generateTrees(int n) {
    if (n == 0)
      return {};
    return generateTreeRange(1, n);
  }
};

// *****

int main() {
  return 0;
}
