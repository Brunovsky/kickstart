#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXJ 1000

vector<vector<int>> mv[64];

void setup() {
    mv[0] = {{0}};
    for (int i = 1; i < 64; i++) {
        int S = mv[i - 1].size();
        int N = S;
        mv[i] = mv[i - 1];

        for (int j = 0; j < S; j++) {
            int last = ++mv[i][j].back();
            if (N < MAXJ && last >= 2) {
                mv[i].push_back(mv[i][j]);
                mv[i].back().back() -= 2;
                mv[i].back().push_back(0);
                N++;
            }
        }
    }
}

auto solve() {
    int N, J;
    cin >> N >> J;
    N -= 4;
    assert(N >= 0 && J <= MAXJ && J <= int(mv[N].size()));

    for (int j = 0; j < J; j++) {
        const auto& spec = mv[N][j];
        string num = "11";
        for (int spaces : spec) {
            num += string(spaces, '0') + "11";
        }
        printf("%s 3 2 5 7 2 3 5 11\n", num.data());
    }
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":\n";
        solve();
    }
    return 0;
}
