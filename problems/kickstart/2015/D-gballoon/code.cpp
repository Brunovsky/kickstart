#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, M, E;
    cin >> N >> M >> E;
    vector<int> speed(M);
    for (int h = 0; h < M; h++)
        cin >> speed[h];

    vector<int> pos(N), height(N);
    for (int i = 0; i < N; i++)
        cin >> pos[i] >> height[i];

    vector<int> cur = height;

    using entry_t = pair<int, int>;
    priority_queue<entry_t> Q;
    int cost = 0;
    constexpr int inf = INT_MAX / 2;

    auto increase_speed = [&](int i) -> bool {
        int d = abs(cur[i] - height[i]) + 1;
        for (int lo = height[i] - d, hi = height[i] + d; lo >= 0 || hi < M; lo--, hi++) {
            int speed_lo = lo >= 0 ? speed[lo] : -inf;
            int speed_hi = hi < M ? speed[hi] : -inf;
            if (max(0, speed[cur[i]]) < max(speed_lo, speed_hi)) {
                cur[i] = speed_lo > speed_hi ? lo : hi;
                return true;
            }
        }
        return false;
    };
    auto decrease_speed = [&](int i) -> bool {
        int d = abs(cur[i] - height[i]) + 1;
        for (int lo = height[i] - d, hi = height[i] + d; lo >= 0 || hi < M; lo--, hi++) {
            int speed_lo = lo >= 0 ? speed[lo] : inf;
            int speed_hi = hi < M ? speed[hi] : inf;
            if (min(0, speed[cur[i]]) > min(speed_lo, speed_hi)) {
                cur[i] = speed_lo < speed_hi ? lo : hi;
                return true;
            }
        }
        return false;
    };

    for (int i = 0; i < N; i++) {
        if (pos[i] == 0)
            continue;
        if (pos[i] > 0 && speed[height[i]] >= 0) {
            if (!decrease_speed(i))
                return "IMPOSSIBLE"s;
        } else if (pos[i] < 0 && speed[height[i]] <= 0) {
            if (!increase_speed(i))
                return "IMPOSSIBLE"s;
        }
        int delta = abs(cur[i] - height[i]);
        int x = abs(pos[i]), vx = abs(speed[cur[i]]);
        int time = (x + vx - 1) / vx;

        cost += delta;
        Q.push({time, i});
    }

    if (cost > E)
        return "IMPOSSIBLE"s;

    while (!Q.empty()) {
        auto [prev_time, i] = Q.top();
        Q.pop();
        int prev_delta = abs(cur[i] - height[i]);

        if (pos[i] > 0 && !decrease_speed(i)) {
            return to_string(prev_time);
        } else if (pos[i] < 0 && !increase_speed(i)) {
            return to_string(prev_time);
        }

        int delta = abs(cur[i] - height[i]);
        int x = abs(pos[i]), vx = abs(speed[cur[i]]);
        int time = (x + vx - 1) / vx;

        cost += delta - prev_delta;
        if (cost > E)
            return to_string(prev_time);
        Q.push({time, i});
    }

    return "0"s;
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
