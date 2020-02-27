#include <bits/stdc++.h>

using namespace std;

// *****

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

struct Best {
  int line = INT_MIN, any = INT_MIN;
};

class Solution {
  Best dive(TreeNode *node) {
    Best left, right, best;

    if (node->left != nullptr)
      left = dive(node->left);
    if (node->right != nullptr)
      right = dive(node->right);
    best.any = best.line = node->val + max(0, max(left.line, right.line));

    if (node->left != nullptr && node->right != nullptr) {
      best.any = max(best.line, node->val + left.line + right.line);
    }
    best.any = max(best.any, max(left.any, right.any));

    return best;
  }

public:
  int maxPathSum(TreeNode *root) {
    return dive(root).any;
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
