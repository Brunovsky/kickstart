#include <bits/stdc++.h>

using namespace std;

// *****

#define ROCK    1
#define PAPER   2
#define SCISS   4
#define CHAR(b) (b == ROCK ? 'R' : (b == SCISS ? 'S' : 'P'))
#define BIT(c)  (c == 'R' ? ROCK : (c == 'S' ? SCISS : PAPER))

#define MAXL 500
#define MAXA 260

int A;
string C[MAXA];

auto solve() {
    cin >> A;
    for (int i = 0; i < A; i++) {
        cin >> C[i];
        while (C[i].size() < MAXL) {
            C[i] += C[i];
        }
        C[i].resize(MAXL);
    }
    string ans;
    for (int b = 0; b < MAXL; b++) {
        int bit = 0;
        for (int i = 0; i < A; i++) {
            if (!C[i].empty()) {
                bit |= BIT(C[i][b]);
            }
        }
        if (bit == (ROCK | PAPER | SCISS)) {
            return "IMPOSSIBLE"s;
        }
        if (bit == ROCK) {
            return ans + CHAR(PAPER);
        } else if (bit == PAPER) {
            return ans + CHAR(SCISS);
        } else if (bit == SCISS) {
            return ans + CHAR(ROCK);
        }
        if (bit == (ROCK | PAPER)) {
            ans += CHAR(PAPER);
            bit = ROCK;
        } else if (bit == (PAPER | SCISS)) {
            ans += CHAR(SCISS);
            bit = PAPER;
        } else if (bit == (SCISS | ROCK)) {
            ans += CHAR(ROCK);
            bit = SCISS;
        }
        for (int i = 0; i < A; i++) {
            if (bit == BIT(C[i][b])) {
                C[i].clear();
            }
        }
    }
    return "IMPOSSIBLE"s;
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
