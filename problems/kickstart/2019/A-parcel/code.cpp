#include <bits/stdc++.h>

using namespace std;

// *****

int R, C;
vector<vector<bool>> towers;
vector<vector<int>> main_grid;

int flood_fill(vector<vector<int>> &grid) {
  int dist = 0;
  vector<pair<int, int>> cur_frontier, next_frontier;

  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      if (towers[r][c]) {
        main_grid[r][c] = 0;
        cur_frontier.push_back({r, c});
      }
    }
  }

  while (!cur_frontier.empty()) {
    ++dist;

    for (auto point : cur_frontier) {
      int r = point.first, c = point.second;

      // north
      if (r > 0 && grid[r - 1][c] > dist) {
        grid[r - 1][c] = dist;
        next_frontier.push_back({r - 1, c});
      }
      // south
      if (r < R - 1 && grid[r + 1][c] > dist) {
        grid[r + 1][c] = dist;
        next_frontier.push_back({r + 1, c});
      }
      // west
      if (c > 0 && grid[r][c - 1] > dist) {
        grid[r][c - 1] = dist;
        next_frontier.push_back({r, c - 1});
      }
      // east
      if (c < C - 1 && grid[r][c + 1] > dist) {
        grid[r][c + 1] = dist;
        next_frontier.push_back({r, c + 1});
      }
    }

    cur_frontier.clear();
    swap(cur_frontier, next_frontier);
  }

  return dist - 1;
}

bool can(int maxray) {
  int sum_min = 0, sum_max = R + C;
  int dif_min = -C, dif_max = R;

  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      if (main_grid[r][c] > maxray) {
        const int sum_cell = r + c, dif_cell = r - c;

        const int sum_min_cell = sum_cell - maxray;
        const int sum_max_cell = sum_cell + maxray;
        const int dif_min_cell = dif_cell - maxray;
        const int dif_max_cell = dif_cell + maxray;

        if (sum_min < sum_min_cell)
          sum_min = sum_min_cell;
        if (sum_max > sum_max_cell)
          sum_max = sum_max_cell;
        if (dif_min < dif_min_cell)
          dif_min = dif_min_cell;
        if (dif_max > dif_max_cell)
          dif_max = dif_max_cell;
      }
    }
    if (sum_min > sum_max || dif_min > dif_max)
      return false;
  }

  //  X <= r + c <= Y        X, Y = sum_min, sum_max
  //  W <= r - c <= Z        W, Z = dif_min, dif_max
  // -Z <= c - r <= -W
  // X + W <= 2r <= Y + Z    even must fit
  // X - Z <= 2c <= Y - W    even must fit
  // => 0 <= X + W <= Y + Z <= 2R - 2
  // => 0 <= X - Z <= Y - W <= 2C - 2

  const int rmin = sum_min + dif_min, rmax = sum_max + dif_max;
  const int cmin = sum_min - dif_max, cmax = sum_max - dif_min;
  const int RMAX = 2 * R - 2, CMAX = 2 * C - 2;

  if (!(0 <= rmax && rmin <= rmax && rmin <= RMAX))
    return false;
  if (!(0 <= cmax && cmin <= cmax && cmin <= CMAX))
    return false;
  if (rmin == rmax && (rmin & 1))
    return false;
  if (cmin == cmax && (cmin & 1))
    return false;
  return true;
}

auto solve() {
  main_grid.assign(R, vector<int>(C, INT32_MAX));
  const int max_dist = flood_fill(main_grid);

  if (max_dist == 0)
    return 0;
  int min = 0, max = max_dist, best = max_dist;

  while (min <= max) {
    int mid = min + (max - min) / 2;

    if (can(mid)) {
      best = mid;
      max = mid - 1;
    } else {
      min = mid + 1;
    }
  }

  return best;
}

// *****

void reparse_test() {
  cin >> R >> C >> ws;
  towers.assign(R, vector<bool>(C, false));
  for (int r = 0; r < R; ++r) {
    string text;
    cin >> text;
    text.resize(C);
    for (int c = 0; c < C; ++c)
      towers[r][c] = text[c] == '1';
  }
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
