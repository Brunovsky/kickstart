#include <bits/stdc++.h>

using namespace std;

// *****

#define ALL(c) begin(c), end(c)

int N, C, M;
vector<array<int, 2>> tickets; // seat, dude
vector<int> by_dude, by_seat;

auto solve() {
    cin >> N >> C >> M;
    tickets.resize(M);
    by_dude.assign(C + 1, 0);
    by_seat.assign(N + 1, 0);

    int R = 0;
    for (auto& [seat, dude] : tickets) {
        cin >> seat >> dude;
        ++by_dude[dude];
        ++by_seat[seat];
        R = max(R, by_dude[dude]);
    }
    sort(ALL(tickets));
    for (int seat = 1, n = 0; seat <= N; seat++) {
        n += by_seat[seat];
        R = max(R, (n + seat - 1) / seat);
    }
    int P = 0; // promotions
    for (int seat = 1; seat <= N; seat++) {
        P += max(0, by_seat[seat] - R);
    }
    return to_string(R) + ' ' + to_string(P);
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
