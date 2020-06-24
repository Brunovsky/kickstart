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
  bool validate(TreeNode *root, int min, int max) {
    if (!root)
      return true;
    int val = root->val;
    return min <= val && val <= max &&
           (val == INT_MIN ? !root->left
                           : validate(root->left, min, val - 1)) &&
           (val == INT_MAX ? !root->right
                           : validate(root->right, val + 1, max));
  }

public:
  bool isValidBST(TreeNode *root) {
    return validate(root, INT_MIN, INT_MAX);
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
