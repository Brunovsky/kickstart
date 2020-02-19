#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int largestRectangleArea(const vector<int> &heights) {
    stack<int> ord;
    vector<int> widths(heights.size(), 0);

    int H = heights.size(), max = 0;

    ord.push(-1);
    for (int i = 0; i < H; ++i) {
      int height = heights[i];

      while (ord.top() >= 0 && height <= heights[ord.top()])
        ord.pop();

      widths[i] = i - ord.top() - 1;

      ord.push(i);
    }

    while (!ord.empty())
      ord.pop();

    ord.push(H);
    for (int i = H - 1; i >= 0; --i) {
      int height = heights[i];

      while (ord.top() < H && height <= heights[ord.top()])
        ord.pop();

      widths[i] += ord.top() - i;
      if (max < widths[i] * height)
        max = widths[i] * height;

      ord.push(i);
    }

    return max;
  }
};

// *****

void test() {
  Solution S;

  cout << S.largestRectangleArea({2, 1, 5, 6, 2, 3}) << endl;
}

// *****

int main() {
  test();
  return 0;
}
