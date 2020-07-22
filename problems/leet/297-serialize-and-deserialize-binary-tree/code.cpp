#include <bits/stdc++.h>

using namespace std;

// *****

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
  TreeNode(int x, TreeNode *left, TreeNode *right)
      : val(x), left(left), right(right) {}
};

#define IS_DIGIT(c) c == '-' || isdigit(c)

class Codec {
  void serialize(TreeNode *root, string &data) {
    if (!root)
      return;
    data += to_string(root->val);
    if (root->left) {
      data.append(1, 'l');
      serialize(root->left, data);
    }
    if (root->right) {
      data.append(1, 'r');
      serialize(root->right, data);
    }
    data.append(1, 'p');
  }

  int deserialize(TreeNode *root, const char *data) {
    int i = 0;
    while (IS_DIGIT(data[i]))
      i++;
    root->val = atoi(data);
    if (data[i] == 'l') {
      root->left = new TreeNode(0);
      i++;
      i += deserialize(root->left, data + i);
    }
    if (data[i] == 'r') {
      root->right = new TreeNode(0);
      i++;
      i += deserialize(root->right, data + i);
    }
    return i + 1;
  }

public:
  // Encodes a tree to a single string.
  string serialize(TreeNode *root) {
    string data;
    serialize(root, data);
    return data;
  }

  // Decodes your encoded data to tree.
  TreeNode *deserialize(string data) {
    if (data.empty())
      return nullptr;
    TreeNode *root = new TreeNode(0);
    deserialize(root, data.data());
    return root;
  }
};

// *****

int main() {
  return 0;
}
