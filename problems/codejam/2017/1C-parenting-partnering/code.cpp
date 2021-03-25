#include <bits/stdc++.h>

using namespace std;

// *****

#define ALL(c) begin(c), end(c)

int NA, NB;
vector<array<int, 3>> day;

auto solve() {
    cin >> NA >> NB;
    day.resize(NA + NB);
    int work[2] = {0, 0};

    for (int i = 0; i < NA + NB; i++) {
        auto& [s, t, who] = day[i];
        cin >> s >> t, who = i < NA;
        work[who] += t - s;
    }

    sort(ALL(day));

    vector<int> sum[2][2] = {};
    int worker = day[NA + NB - 1][2];
    int time = day[NA + NB - 1][1] - 24 * 60;
    int swaps = 0;

    for (int i = 0; i < NA + NB; i++) {
        auto [s, t, who] = day[i];
        if (worker != who || s != time)
            sum[worker][who].push_back(s - time);
        swaps += worker != who;
        worker = who, time = t;
    }
    for (int from = 0; from <= 1; from++)
        for (int to = 0; to <= 1; to++)
            sort(ALL(sum[from][to]), greater<>{});

    // from = to = i
    for (int i = 0; i <= 1; i++) {
        while (!sum[i][i].empty() && work[i] < 720) {
            int& gap = sum[i][i].back();
            assert(gap > 0);
            if (gap <= 720 - work[i])
                work[i] += gap, sum[i][i].pop_back();
            else
                gap -= 720 - work[i], work[i] = 720;
        }
        swaps += 2 * sum[i][i].size();
        work[!i] += accumulate(ALL(sum[i][i]), 0);
    }

    assert(work[0] <= 720 && work[1] <= 720);

    return swaps;
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
