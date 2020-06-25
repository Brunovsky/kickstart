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
  vector<int> inorderTraversal(TreeNode *root) {
    vector<int> ordered;
    // recursiveTraversal(root, ordered);
    iterativeTraversal(root, ordered);
    return ordered;
  }

  void iterativeTraversal(TreeNode *root, vector<int> &ordered) {
    stack<TreeNode *> nodes;
    while (root) {
      nodes.push(root);
      root = root->left;
    }
    while (!nodes.empty()) {
      TreeNode *node = nodes.top();
      nodes.pop();
      ordered.push_back(node->val);
      node = node->right;
      while (node) {
        nodes.push(node);
        node = node->left;
      };
    }
  }

  void recursiveTraversal(TreeNode *root, vector<int> &ordered) {
    if (!root)
      return;
    recursiveTraversal(root->left, ordered);
    ordered.push_back(root->val);
    recursiveTraversal(root->right, ordered);
  }
};

// *****

int main() {
  return 0;
}
