#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

i32 R, C;
vector<vector<bool>> towers;
vector<vector<i32>> main_grid;

i32 flood_fill(vector<vector<i32>>& grid) {
  i32 dist = 0;
  vector<pair<i32, i32>> cur_frontier, next_frontier;

  for (i32 r = 0; r < R; ++r) {
    for (i32 c = 0; c < C; ++c) {
      if (towers[r][c]) {
        main_grid[r][c] = 0;
        cur_frontier.push_back({r, c});
      }
    }
  }

  while (!cur_frontier.empty()) {
    ++dist;

    for (auto point : cur_frontier) {
      i32 r = point.first, c = point.second;

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

bool can(i32 maxray) {
  i32 sum_min = 0, sum_max = R + C;
  i32 dif_min = -C, dif_max = R;

  for (i32 r = 0; r < R; ++r) {
    for (i32 c = 0; c < C; ++c) {
      if (main_grid[r][c] > maxray) {
        const i32 sum_cell = r + c, dif_cell = r - c;

        const i32 sum_min_cell = sum_cell - maxray;
        const i32 sum_max_cell = sum_cell + maxray;
        const i32 dif_min_cell = dif_cell - maxray;
        const i32 dif_max_cell = dif_cell + maxray;

        if (sum_min < sum_min_cell) sum_min = sum_min_cell;
        if (sum_max > sum_max_cell) sum_max = sum_max_cell;
        if (dif_min < dif_min_cell) dif_min = dif_min_cell;
        if (dif_max > dif_max_cell) dif_max = dif_max_cell;
      }
    }
    if (sum_min > sum_max || dif_min > dif_max) return false;
  }

  //  X <= r + c <= Y        X, Y = sum_min, sum_max
  //  W <= r - c <= Z        W, Z = dif_min, dif_max
  // -Z <= c - r <= -W
  // X + W <= 2r <= Y + Z    even must fit
  // X - Z <= 2c <= Y - W    even must fit
  // => 0 <= X + W <= Y + Z <= 2R - 2
  // => 0 <= X - Z <= Y - W <= 2C - 2

  const i32 rmin = sum_min + dif_min, rmax = sum_max + dif_max;
  const i32 cmin = sum_min - dif_max, cmax = sum_max - dif_min;
  const i32 RMAX = 2 * R - 2, CMAX = 2 * C - 2;

  if (!(0 <= rmax && rmin <= rmax && rmin <= RMAX)) return false;
  if (!(0 <= cmax && cmin <= cmax && cmin <= CMAX)) return false;
  if (rmin == rmax && (rmin & 1)) return false;
  if (cmin == cmax && (cmin & 1)) return false;
  return true;
}

auto solve() {
  main_grid.assign(R, vector<i32>(C, INT32_MAX));
  const i32 max_dist = flood_fill(main_grid);

  if (max_dist == 0) return 0;
  i32 min = 0, max = max_dist, best = max_dist;

  while (min <= max) {
    i32 mid = min + (max - min) / 2;

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

void prep() {}

void parse_test() {
  cin >> R >> C >> ws;
  towers.resize(R);
  for (i32 r = 0; r < R; ++r) {
    std::string text;
    cin >> text;
    text.resize(C);
    towers[r].resize(C);
    for (i32 c = 0; c < C; ++c) towers[r][c] = text[c] == '1';
  }
}

void reset_test() {
  towers.clear();
}

void print_test(u64 casenum, i32 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  prep();
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
