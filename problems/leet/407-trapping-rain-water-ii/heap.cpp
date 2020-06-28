#include <bits/stdc++.h>

using namespace std;

// *****

struct point {
  int r, c;
};

bool operator<(point lhs, point rhs) {
  return lhs.r < rhs.r || (lhs.r == rhs.r && lhs.c < rhs.c);
}

constexpr point adj[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

class Solution {
public:
  int trapRainWater(vector<vector<int>> &H) {
    if (H.empty())
      return 0;
    int R = H.size(), C = H[0].size();
    if (R <= 2 || C <= 2)
      return 0;

    using P = pair<int, point>;
    priority_queue<P, vector<P>, greater<P>> valleys;
    uint8_t F[R][C];
    memset(F, 0, sizeof(F));

    for (int r = 0; r < R; ++r) {
      valleys.push({H[r][0], {r, 0}});
      valleys.push({H[r][C - 1], {r, C - 1}});
    }
    for (int c = 0; c < C; ++c) {
      valleys.push({H[0][c], {0, c}});
      valleys.push({H[R - 1][c], {R - 1, c}});
    }

    int water = 0;
    while (!valleys.empty()) {
      P top = valleys.top();
      valleys.pop();
      point V = top.second;
      if (F[V.r][V.c])
        continue;
      F[V.r][V.c] = true;
      int hV = top.first;

      for (point off : adj) {
        int r = V.r + off.r, c = V.c + off.c;
        if (r <= 0 || c <= 0 || r >= R - 1 || c >= C - 1 || F[r][c])
          continue;
        int &hN = H[r][c];
        if (hN < hV) {
          water += hV - hN;
          hN = hV;
        }
        valleys.push({hN, {r, c}});
      }
    }
    return water;
  }
};

// *****

int main() {
  vector<vector<int>> H = {
      {4, 7, 8, 2, 6, 1, 3, 4, 1, 1, 2}, {5, 1, 2, 6, 4, 5, 1, 2, 5, 4, 3},
      {5, 2, 4, 1, 5, 1, 2, 9, 2, 3, 2}, {1, 3, 3, 2, 1, 8, 1, 1, 1, 2, 4},
      {5, 2, 4, 2, 9, 7, 8, 3, 5, 6, 2}, {3, 1, 5, 4, 1, 9, 1, 6, 4, 3, 4},
      {3, 2, 3, 2, 2, 8, 2, 5, 3, 2, 6}, {1, 4, 4, 4, 3, 2, 4, 3, 7, 4, 1},
  };
  int expected = 43;
  auto actual = Solution{}.trapRainWater(H);
  if (actual != expected) {
    printf("Test failed\n");
    printf("> Expected: %d\n", expected);
    printf("> Actual:   %d\n", actual);
  }
  return 0;
}
