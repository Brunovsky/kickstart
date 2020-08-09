#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  int height(TreeNode *root) {
    if (!root)
      return 0;
    int L = height(root->left);
    int R = height(root->right);
    if (L == -1 || R == -1)
      return -1;
    int avl = abs(R - L);
    return avl <= 1 ? 1 + max(R, L) : -1;
  }

public:
  bool isBalanced(TreeNode *root) {
    return height(root) != -1;
  }
};

// *****

int main() {
  return 0;
}
