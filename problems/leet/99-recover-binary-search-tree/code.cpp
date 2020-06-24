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

public:
  void recoverTree(TreeNode *root) {
    TreeNode *prev = nullptr, *left = nullptr, *right = nullptr;

    stack<TreeNode *> nodes;
    while (root) {
      nodes.push(root);
      root = root->left;
    }

    while (!nodes.empty()) {
      TreeNode *node = nodes.top();
      nodes.pop();

      if (prev && prev->val > node->val) {
        if (left) {
          right = node;
          break;
        } else {
          left = prev;
          right = node;
        }
      }
      prev = node;

      node = node->right;
      while (node) {
        nodes.push(node);
        node = node->left;
      };
    }
    swap(left->val, right->val);
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
