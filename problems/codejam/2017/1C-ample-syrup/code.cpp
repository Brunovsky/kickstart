#include <bits/stdc++.h>

using namespace std;

// *****

struct Pancake {
    int i;
    long R, H;
    long side() const { return 2 * R * H; }
    bool operator<(Pancake other) const {
        return side() != other.side() ? side() > other.side() : i > other.i;
    }
};

#define MAXN 1000

int N, K;
Pancake pancakes[MAXN];

auto solve() {
    cin >> N >> K;
    for (int i = 0; i < N; i++) {
        pancakes[i].i = i, cin >> pancakes[i].R >> pancakes[i].H;
    }
    sort(pancakes, pancakes + N);

    long total = 0, width = 0;
    for (int i = 0; i < K - 1; i++) {
        total += pancakes[i].side();
        width = max(width, pancakes[i].R);
    }

    long best = 0;
    for (int i = K - 1; i < N; i++) {
        long radius = max(width, pancakes[i].R);
        best = max(best, total + pancakes[i].side() + radius * radius);
    }

    return M_PI * best;
}

// *****

int main() {
    cout << setprecision(7) << fixed << showpoint;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
