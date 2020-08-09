#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  TreeNode *searchBST(TreeNode *root, int val) {
    while (root) {
      if (root->val == val) {
        break;
      } else if (root->val < val) {
        root = root->right;
      } else {
        root = root->left;
      }
    }
    return root;
  }
};

// *****

int main() {
  return 0;
}
