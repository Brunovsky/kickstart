#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int DAY = 86400;

int readtime() {
    int h, m, s;
    auto n = scanf("%d:%d:%d", &h, &m, &s);
    assert(n == 3);
    return 3600 * h + 60 * m + s;
}

auto solve() {
    int G0 = readtime(); // wake up
    int B1 = readtime(); // work
    int G1 = readtime(); // home
    int B0 = readtime(); // bedtime
    int X = readtime();
    long D;
    cin >> D;
    if (G0 > X || DAY - B0 >= X)
        return -1L;
    if (G1 - B1 >= X)
        return -1L;
    if (G0 + DAY - B0 >= X && D > 1)
        return -1L;

    unordered_map<int, pair<int, long>> save;
    int t = 0;
    long d = 0, feed = 0;

    while (d < D && !save.count(t)) {
        save[t] = {d, feed};
        while (t + X < B1)
            t += X, feed++;
        if (t < B1 - 1 && t + X < G1)
            t = B1 - 1, feed++;
        while (t + X < B0)
            t += X, feed++;
        if (d + 1 < D && t < B0 - 1 && t + X < G0 + DAY)
            t = B0 - 1, feed++;
        if (d + 1 == D && t < B0 - 1 && t + X < DAY)
            t = B0 - 1, feed++;
        d++, t -= DAY;
    }
    if (d < D) {
        auto [id, ifeed] = save.at(t);
        long inc_d = d - id, inc_feed = feed - ifeed;
        long skip = (D - d - 1) / inc_d;
        d += skip * inc_d;
        feed += skip * inc_feed;
    }
    while (d < D) {
        while (t + X < B1)
            t += X, feed++;
        if (t < B1 - 1 && t + X < G1)
            t = B1 - 1, feed++;
        while (t + X < B0)
            t += X, feed++;
        if (d + 1 < D && t < B0 - 1 && t + X < G0 + DAY)
            t = B0 - 1, feed++;
        if (d + 1 == D && t < B0 - 1 && t + X < DAY)
            t = B0 - 1, feed++;
        d++, t -= DAY;
    }
    return feed;
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
