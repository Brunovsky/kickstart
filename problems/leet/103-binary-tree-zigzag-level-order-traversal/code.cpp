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
public:
  void traverse(vector<vector<int>> &levels, TreeNode *root, unsigned depth) {
    if (!root)
      return;
    if (depth >= levels.size())
      levels.resize(depth + 1);
    levels[depth].push_back(root->val);
    traverse(levels, root->left, depth + 1);
    traverse(levels, root->right, depth + 1);
  }

public:
  vector<vector<int>> zigzagLevelOrder(TreeNode *root) {
    vector<vector<int>> levels;
    traverse(levels, root, 0);
    for (uint i = 1; i < levels.size(); i += 2) {
      reverse(levels[i].begin(), levels[i].end());
    }
    return levels;
  }
};

// *****

int main() {
  return 0;
}
