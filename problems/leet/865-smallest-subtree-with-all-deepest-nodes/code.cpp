#include <bits/stdc++.h>

using namespace std;

// *****

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
  tuple<TreeNode *, int> dive(TreeNode *root, int depth) {
    if (!root)
      return {nullptr, depth - 1};

    auto [lhs, depth_lhs] = dive(root->left, depth + 1);
    auto [rhs, depth_rhs] = dive(root->right, depth + 1);

    if (depth_lhs > depth_rhs)
      return {lhs, depth_lhs};
    if (depth_lhs < depth_rhs)
      return {rhs, depth_rhs};
    return {root, depth_lhs};
  }

public:
  TreeNode *subtreeWithAllDeepest(TreeNode *root) {
    auto [node, depth] = dive(root, 0);
    return node;
  }
};

// *****

void test() {
  TreeNode *root = new TreeNode(3);
  root->left = new TreeNode(5);
  root->left->left = new TreeNode(6);
  root->left->right = new TreeNode(2);
  root->left->right->left = new TreeNode(7);
  root->left->right->right = new TreeNode(4);
  root->right = new TreeNode(1);
  root->right->left = new TreeNode(0);
  root->right->right = new TreeNode(8);

  TreeNode *node = Solution{}.subtreeWithAllDeepest(root);
  if (node && node->val == 2)
    cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
  else
    cout << "Test failed (expected 2)\n";
}

// *****

int main() {
  test();
  return 0;
}
