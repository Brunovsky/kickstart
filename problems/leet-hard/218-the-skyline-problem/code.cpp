#include <bits/stdc++.h>

using namespace std;

// ***** Segment tree solution, for practice

struct range {
  int L, R, y = 0;
};

class Solution {
  int O, P;
  vector<range> tree;
  vector<range>::iterator obegin, oend;

  void pushup(int i) {
    if (i < O) {
      pushup(i << 1);
      pushup(i << 1 | 1);
      tree[i].L = tree[i << 1].L;
      tree[i].R = tree[i << 1 | 1].R;
    }
  }

  void insert(int i, range build) {
    if (tree[i].y >= build.y) return;

    // overlap
    if (build.L < tree[i].R && tree[i].L < build.R) {
      // contain
      if (build.L <= tree[i].L && tree[i].R <= build.R) {
        if (tree[i].y < build.y) tree[i].y = build.y;
      } else {
        assert(i < O);
        insert(i << 1, build);
        insert(i << 1 | 1, build);
      }
    }
  }

  void pushdown(int i) {
    tree[i].y = max(tree[i].y, tree[i >> 1].y);
    if (i < O) {
      pushdown(i << 1);
      pushdown(i << 1 | 1);
    }
  }

 public:
  vector<vector<int>> getSkyline(const vector<vector<int>>& buildings) {
    if (buildings.empty()) return {};

    vector<int> endp;

    for (const auto& building : buildings) {
      endp.push_back(building[0]);
      endp.push_back(building[1]);
    }
    sort(endp.begin(), endp.end());
    endp.erase(unique(endp.begin(), endp.end()), endp.end());
    O = endp.size() - 1;
    P = 1;
    while (P < O) P <<= 1;

    tree.assign(2 * O, {});
    obegin = tree.begin() + O;
    oend = tree.end();

    for (int i = 0; i < O; ++i) {
      tree[i + O].L = endp[i];
      tree[i + O].R = endp[i + 1];
    }

    rotate(obegin, obegin + (2 * O - P), oend);  // align tree
    pushup(1);
    for (const auto& building : buildings) {
      insert(1, {building[0], building[1], building[2]});
    }
    pushdown(1);
    rotate(obegin, obegin + (P - O), oend);  // align array

    vector<vector<int>> skyline;
    auto left = obegin;

    skyline.push_back({left->L, left->y});
    for (auto right = obegin + 1; right != oend; ++left, ++right) {
      if (left->y != right->y) {
        skyline.push_back({right->L, right->y});
      }
    }
    skyline.push_back({left->R, 0});

    return skyline;
  }
};

// align leaves with array
// rotate(obegin, obegin + (P - O), oend);

// *****

void test() {
  Solution S;

  vector<vector<int>> buildings = {
    {2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8},
  };

  auto skyline = S.getSkyline(buildings);

  for (auto range : skyline) {
    cout << range[0] << ' ' << range[1] << endl;
  }
}

// *****

int main() {
  test();
  return 0;
}
