#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int dd[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

#define MAXR 250
#define MAXC 250

int R, C;
bool towers[MAXR][MAXC];
int grid[MAXR][MAXC];

inline bool within_bounds(int r, int c) {
    return r >= 0 && r < R && c >= 0 && c < C;
}

int flood_fill() {
    vector<pair<int, int>> frontier, next_frontier;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            if (towers[r][c]) {
                grid[r][c] = 0;
                frontier.push_back({r, c});
            }
        }
    }

    int dist = 0;
    while (!frontier.empty()) {
        dist++;
        for (auto point : frontier) {
            int pr = point.first, pc = point.second;
            for (auto d : dd) {
                int r = pr + d[0], c = pc + d[1];
                if (within_bounds(r, c) && grid[r][c] > dist) {
                    grid[r][c] = dist;
                    next_frontier.push_back({r, c});
                }
            }
        }
        frontier.clear();
        swap(frontier, next_frontier);
    }
    return dist - 1;
}

bool can(int maxray) {
    int sum_min = 0, sum_max = R + C;
    int dif_min = -C, dif_max = R;

    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            if (grid[r][c] > maxray) {
                sum_min = max(sum_min, r + c - maxray);
                sum_max = min(sum_max, r + c + maxray);
                dif_min = max(dif_min, r - c - maxray);
                dif_max = min(dif_max, r - c + maxray);
            }
        }
        if (sum_min > sum_max || dif_min > dif_max) {
            return false;
        }
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

    if (!(0 <= rmax && rmin <= rmax && rmin <= RMAX)) {
        return false;
    }
    if (!(0 <= cmax && cmin <= cmax && cmin <= CMAX)) {
        return false;
    }
    if (rmin == rmax && (rmin & 1)) {
        return false;
    }
    if (cmin == cmax && (cmin & 1)) {
        return false;
    }
    return true;
}

auto solve() {
    cin >> R >> C >> ws;
    memset(towers, 0, sizeof(towers));
    memset(grid, 0x7f, sizeof(grid));
    for (int r = 0; r < R; ++r) {
        string text;
        cin >> text;
        for (int c = 0; c < C; ++c) {
            towers[r][c] = text[c] == '1';
        }
    }

    int max_dist = flood_fill();
    if (max_dist == 0) {
        return 0;
    }

    int min = 0, max = max_dist;
    while (min <= max) {
        int mid = (min + max) / 2;
        if (can(mid)) {
            max = mid - 1;
        } else {
            min = mid + 1;
        }
    }
    return max + 1;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
