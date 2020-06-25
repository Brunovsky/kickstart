#include <bits/stdc++.h>

using namespace std;

// *****

struct union_set {
private:
  int find(int x) {
    assert(contains(x));
    int root = x;
    while (m[root].parent != root)
      root = m[root].parent;
    while (m[x].parent != root) {
      int parent = m[x].parent;
      m[x].parent = root;
      x = parent;
    }
    return root;
  }
  struct entry_t {
    int parent, size = 1;
  };

  std::unordered_map<int, entry_t> m;

public:
  void make_set(int x) {
    if (m.count(x))
      return;
    m[x] = {x, 1};
  }

  bool contains(int x) {
    return m.count(x);
  }

  int make_union(int x, int y) {
    int xroot = find(x);
    int yroot = find(y);
    if (xroot == yroot)
      return m[xroot].size;

    // xroot.size >= yroot.size
    if (m[xroot].size < m[yroot].size)
      swap(xroot, yroot);
    m[yroot].parent = xroot;
    m[xroot].size += m[yroot].size;
    return m[xroot].size;
  }
};

class Solution {
public:
  auto longestConsecutive(const vector<int> &nums) {
    union_set uset;
    int max_size = 0;

    for (size_t i = 0; i < nums.size(); ++i) {
      int n = nums[i];
      if (uset.contains(n))
        continue;
      int size = 1;
      uset.make_set(n);
      if (uset.contains(n + 1))
        size = uset.make_union(n, n + 1);
      if (uset.contains(n - 1))
        size = uset.make_union(n, n - 1);
      if (max_size < size)
        max_size = size;
    }

    return max_size;
  }
};

// *****

int main() {
  vector<int> example1 = {100, 4, 200, 1, 3, 2};
  auto n = Solution{}.longestConsecutive(example1);
  if (n != 4) {
    printf("Failed test\n");
    printf("> Expected: 4\n");
    printf("> Actual:   %d\n", n);
  }

  return 0;
}
