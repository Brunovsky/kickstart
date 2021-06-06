#include <bits/stdc++.h>

using namespace std;

// *****

auto solve1(int N, int L) { return max(0, (L - 1) - 2 * (L - N)); }

auto solve() {
    int R, C, N;
    cin >> R >> C >> N;
    if (R == 1)
        return solve1(N, C);
    if (C == 1)
        return solve1(N, R);

    if (R % 2 == 0 || C % 2 == 0) {
        // R=4,C=8, 2s=2, 3s=8, 4s=6
        // 2.3.3.3.
        // .4.4.4.3
        // 3.4.4.4.
        // .3.3.3.2
        int add4 = (R - 2) * (C - 2) / 2;
        int add3 = R + C - 4;
        int add2 = 2;
        int fill = R * C - add2 - add3 - add4;
        int M = N - fill;

        int cost = 0;
        if (0 <= M && M <= add2)
            cost = 2 * M;
        else if (0 <= M && M <= add2 + add3)
            M -= add2, cost = 2 * add2 + 3 * M;
        else if (0 <= M && M <= add2 + add3 + add4)
            M -= add2 + add3, cost = 2 * add2 + 3 * add3 + 4 * M;
        else
            assert(M < 0);

        return cost;
    } else {
        // case 1
        // R=5,C=9, 2s=4, 3s=8, 4s=11, filled=22
        // 2.3.3.3.2
        // .4.4.4.4.
        // 3.4.4.4.3
        // .4.4.4.4.
        // 2.3.3.3.2
        int add4 = (R - 2) * (C - 2) / 2 + 1;
        int add3 = R + C - 6;
        int add2 = 4;
        int fill = R * C - add2 - add3 - add4;
        int M = N - fill;

        int cost_0 = 0;
        if (0 <= M && M <= add2)
            cost_0 = 2 * M;
        else if (0 <= M && M <= add2 + add3)
            M -= add2, cost_0 = 2 * add2 + 3 * M;
        else if (0 <= M && M <= add2 + add3 + add4)
            M -= add2 + add3, cost_0 = 2 * add2 + 3 * add3 + 4 * M;
        else
            assert(M < 0);

        // case 2
        // R=5,C=9, 2s=0, 3s=12, 4s=10, filled=23
        // .3.3.3.3.
        // 3.4.4.4.3
        // .4.4.4.4.
        // 3.4.4.4.3
        // .3.3.3.3.
        add4 = (R - 2) * (C - 2) / 2;
        add3 = R + C - 2;
        add2 = 0;
        fill = R * C - add2 - add3 - add4;
        M = N - fill;

        int cost_1 = 0;
        if (0 <= M && M <= add2)
            cost_1 = 2 * M;
        else if (0 <= M && M <= add2 + add3)
            M -= add2, cost_1 = 2 * add2 + 3 * M;
        else if (0 <= M && M <= add2 + add3 + add4)
            M -= add2 + add3, cost_1 = 2 * add2 + 3 * add3 + 4 * M;
        else
            assert(M < 0);

        return min(cost_0, cost_1);
    }
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
