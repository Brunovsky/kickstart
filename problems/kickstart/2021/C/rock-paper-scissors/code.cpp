#include <bits/stdc++.h>

using namespace std;

// *****

using dp_t = pair<double, string>;
double best_score[4];
string best_play[4];
constexpr double rat[4] = {1.0, 0.5, 0.1, 0.0};
dp_t dp[4][61][61][61];

template <typename T>
void setmax(T& a, T b) {
    a = max(a, b);
}

void setup_dp() {
    // 0->E=W   1->E=W/2  2->E=W/10  3->E=0

    for (int t : {0, 1, 2, 3}) {
        dp[t][1][0][0] = make_pair((1.0 + rat[t]) / 3.0, "S"s);
        dp[t][0][1][0] = make_pair((1.0 + rat[t]) / 3.0, "R"s);
        dp[t][0][0][1] = make_pair((1.0 + rat[t]) / 3.0, "P"s);

        for (int sum = 2; sum <= 60; sum++) {
            for (int r = 0; r <= sum; r++) {
                for (int p = 0, s = sum - r; p + r <= sum; p++, s--) {
                    int minus = sum - 1;
                    if (r > 0) { // we played SCISSORS
                        auto [q, play] = dp[t][r - 1][p][s];
                        auto win = 1.0 * p / minus + rat[t] * (s - 1) / minus;
                        setmax(dp[t][r][p][s], dp_t{q + win, play + "S"});
                    }
                    if (p > 0) { // we played ROCK
                        auto [q, play] = dp[t][r][p - 1][s];
                        auto win = 1.0 * s / minus + rat[t] * (r - 1) / minus;
                        setmax(dp[t][r][p][s], dp_t{q + win, play + "R"});
                    }
                    if (s > 0) { // we played PAPER
                        auto [q, play] = dp[t][r][p][s - 1];
                        auto win = 1.0 * r / minus + rat[t] * (p - 1) / minus;
                        setmax(dp[t][r][p][s], dp_t{q + win, play + "P"});
                    }
                    if (sum == 60 && dp[t][r][p][s].first > best_score[t]) {
                        best_score[t] = dp[t][r][p][s].first;
                        best_play[t] = dp[t][r][p][s].second;
                    }
                }
            }
        }

        fprintf(stderr, "best[%d]: %s\n", t, best_play[t].c_str());
        fprintf(stderr, "score[%d]: %lf\n", t, 500 * best_score[t]);
    }

    double total = accumulate(best_score, best_score + 4, 0.0);
    fprintf(stderr, "total: %lf\n", total * 500 / 4);
    // this gives 15640 which doesn't make any sense lol
}

auto solve() {
    int W, E;
    cin >> W >> E;

    if (E == W) {
        return best_play[0];
    } else if (E == W / 2) {
        return best_play[1];
    } else if (E == W / 10) {
        return best_play[2];
    } else {
        return best_play[3];
    }
}

// *****

int main() {
    setup_dp();
    unsigned T, X;
    cin >> T >> X >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
