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
  TreeNode *build(vector<int> &inorder, vector<int> &postorder, int ib, int ie, int pb,
                  int pe) {
    if (ib > ie || pb > pe) {
      return nullptr;
    }
    TreeNode *root = new TreeNode(postorder[pe]);
    int ir = find(begin(inorder) + ib, begin(inorder) + ie, root->val) - begin(inorder);
    int lsize = ir - ib;
    int rsize = ie - ir;
    root->left = build(inorder, postorder, ib, ir - 1, pb, pb + lsize - 1);
    root->right = build(inorder, postorder, ir + 1, ie, pb + lsize, pe - 1);
    return root;
  }

public:
  TreeNode *buildTree(vector<int> &inorder, vector<int> &postorder) {
    int n = inorder.size();
    return build(inorder, postorder, 0, n - 1, 0, n - 1);
  }
};

// *****

int main() {
  return 0;
}
