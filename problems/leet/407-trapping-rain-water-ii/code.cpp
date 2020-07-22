#include <bits/stdc++.h>

using namespace std;

// *****

struct point {
  int r, c;
};

constexpr point adj[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

class Solution {
public:
  int trapRainWater(vector<vector<int>> &H) {
    if (H.empty())
      return 0;
    int R = H.size();
    int C = H[0].size();
    if (R <= 2 || C <= 2)
      return 0;

    auto cmp = [&H](point a, point b) {
      return tie(H[a.r][a.c], a.r, a.c) < tie(H[b.r][b.c], b.r, b.c);
    };

    set<point, function<bool(point, point)>> valleys(cmp);
    vector<vector<uint8_t>> F(R, vector<uint8_t>(C, 0));

    for (int r = 0; r < R; ++r) {
      valleys.insert({r, 0});
      valleys.insert({r, C - 1});
      F[r][0] = F[r][C - 1] = true;
    }
    for (int c = 0; c < C; ++c) {
      valleys.insert({0, c});
      valleys.insert({R - 1, c});
      F[0][c] = F[R - 1][c] = true;
    }

    int water = 0;
    while (!valleys.empty()) {
      point V = *valleys.begin();
      valleys.erase(valleys.begin());
      F[V.r][V.c] = true;
      int hV = H[V.r][V.c];

      for (point off : adj) {
        int r = V.r + off.r, c = V.c + off.c;
        if (r < 0 || c < 0 || r >= R || c >= C || F[r][c])
          continue;
        int &hN = H[r][c];
        if (hN < hV) {
          water += hV - hN;
          valleys.erase({r, c});
          hN = hV;
        }
        valleys.insert({r, c});
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
