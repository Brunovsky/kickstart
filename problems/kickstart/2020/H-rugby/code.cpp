#include <bits/stdc++.h>

using namespace std;

// *****

struct P {
    long x, y;
};

int N;
vector<P> pos;

long cost(long x, long y) {
    long c = 0;
    for (int i = 0; i < N; i++) {
        c += abs(pos[i].x - (x + i));
        c += abs(pos[i].y - y);
    }
    return c;
}

auto solve() {
    cin >> N;
    pos.assign(N, {});
    for (int i = 0; i < N; i++)
        cin >> pos[i].x >> pos[i].y;

    sort(begin(pos), end(pos), [&](P lhs, P rhs) {
        return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
    });

    // X first
    long L, R;

    L = -1'010'000'000, R = 1'010'000'000;
    while (L <= R) {
        long M1 = long(floor((2 * L + R) / 3.0));
        long M2 = long(ceil((L + 2 * R) / 3.0));
        long costM1 = cost(M1, 0);
        long costM2 = cost(M2, 0);
        if (costM1 < costM2) {
            R = M2 - 1;
        } else {
            L = M1 + 1;
        }
    }
    long x = (cost(L, 0) < cost(R, 0)) ? L : R;

    L = -1'010'000'000, R = 1'010'000'000;
    while (L <= R) {
        long M1 = long(floor((2 * L + R) / 3.0));
        long M2 = long(ceil((L + 2 * R) / 3.0));
        long costM1 = cost(x, M1);
        long costM2 = cost(x, M2);
        if (costM1 < costM2) {
            R = M2 - 1;
        } else {
            L = M1 + 1;
        }
    }
    long y = (cost(x, L) < cost(x, R)) ? L : R;

    return cost(x, y);
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
