#include <bits/stdc++.h>

using namespace std;

// *****

// P1            2d            P2
//  +--------------------------+
//   \           |            /
//     \         |          /
//       \       | h      /
//         \     |      /
//      r    \   |    /    r
//             \ |  /
//               \/ <- Center of circle

class Solution {
public:
  int numPoints(const vector<vector<int>> &points, int r) {
    int ans = 1;
    int P = points.size();

    for (int i1 = 0; i1 < P; i1++) {
      int x1 = points[i1][0], y1 = points[i1][1];
      int i1cnt = 1;

      for (int i2 = i1 + 1; i2 < P; i2++) {
        int x2 = points[i2][0], y2 = points[i2][1];
        if (x1 == x2 && y1 == y2) {
          i1cnt++;
          continue;
        }
        int dx = x1 - x2, dy = y1 - y2;
        int d2 = dx * dx + dy * dy; // distance P1P2 squared
        if (d2 > 4 * r * r) {
          continue;
        }
        double d = sqrt(d2);
        double h = sqrt(r * r - d2 / 4.0);
        double px = -h * dy / d;
        double py = h * dx / d;

        for (int j : {-1, 1}) {
          double cx = 0.5 * (x1 + x2) + j * px;
          double cy = 0.5 * (y1 + y2) + j * py;

          int cnt = 0;
          for (int i = 0; i < P; i++) {
            int x = points[i][0], y = points[i][1];
            double dcx = cx - x, dcy = cy - y;
            double dp = dcx * dcx + dcy * dcy;
            cnt += dp <= r * r + 1e-9;
          }
          ans = max(ans, cnt);
        }
      }
      ans = max(ans, i1cnt);
    }
    return ans;
  }
};

// *****

Solution sol;

int main() {
  cout << fixed << setprecision(12);
  cout << sol.numPoints({{-2, 0}, {2, 0}, {0, 2}, {0, -2}}, 2) << '\n';
  return 0;
}
