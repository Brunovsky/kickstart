#include <bits/stdc++.h>

using namespace std;

// *****

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
  void visit(TreeNode *node, vector<int> &path, vector<vector<int>> &ans, int path_sum,
             int sum) {
    path.push_back(node->val);
    path_sum += node->val;
    if (!node->left && !node->right) {
      if (path_sum == sum) {
        ans.push_back(path);
      }
    }
    if (node->left) {
      visit(node->left, path, ans, path_sum, sum);
    }
    if (node->right) {
      visit(node->right, path, ans, path_sum, sum);
    }
    path.pop_back();
  }

public:
  vector<vector<int>> pathSum(TreeNode *root, int sum) {
    vector<int> path;
    vector<vector<int>> ans;
    if (root) {
      visit(root, path, ans, 0, sum);
    }
    return ans;
  }
};

// *****

int main() {
  return 0;
}
