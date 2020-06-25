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
  void dive(TreeNode *node, int node_depth, int &sum, int &sum_depth) {
    if (node == nullptr)
      return;
    dive(node->left, node_depth + 1, sum, sum_depth);
    dive(node->right, node_depth + 1, sum, sum_depth);
    if (node_depth < sum_depth)
      return;
    if (node_depth == sum_depth)
      sum += node->val;
    if (node_depth > sum_depth) {
      sum = node->val;
      sum_depth = node_depth;
    }
  }

public:
  int deepestLeavesSum(TreeNode *root) {
    int sum = 0, sum_depth = 0;
    dive(root, 0, sum, sum_depth);
    return sum;
  }
};

// *****

int main() {
  return 0;
}
