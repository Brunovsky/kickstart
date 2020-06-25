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
  void explore(TreeNode *node, int depth, vector<int> &seen) const {
    if (node == nullptr)
      return;
    if (depth >= int(seen.size()))
      seen.push_back(node->val);
    explore(node->right, depth + 1, seen);
    explore(node->left, depth + 1, seen);
  }

public:
  vector<int> rightSideView(TreeNode *root) {
    vector<int> seen;
    explore(root, 0, seen);
    return seen;
  }
};

// *****

int main() {
  return 0;
}
