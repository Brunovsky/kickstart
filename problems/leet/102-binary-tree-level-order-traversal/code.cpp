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
  void traverse(vector<vector<int>> &levels, TreeNode *root, int depth) {
    if (!root)
      return;
    if (depth >= int(levels.size()))
      levels.resize(depth + 1);
    levels[depth].push_back(root->val);
    traverse(levels, root->left, depth + 1);
    traverse(levels, root->right, depth + 1);
  }

public:
  vector<vector<int>> levelOrder(TreeNode *root) {
    vector<vector<int>> levels;
    traverse(levels, root, 0);
    return levels;
  }
};

// *****

int main() {
  return 0;
}
