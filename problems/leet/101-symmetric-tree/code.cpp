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
  bool isSymmetric(TreeNode *left, TreeNode *right) const {
    if (!left && !right)
      return true;
    if (!left || !right || left->val != right->val)
      return false;
    return isSymmetric(left->left, right->right) &&
           isSymmetric(left->right, right->left);
  }

public:
  bool isSymmetric(TreeNode *root) {
    return !root || isSymmetric(root->left, root->right);
  }
};

// *****

int main() {
  return 0;
}
