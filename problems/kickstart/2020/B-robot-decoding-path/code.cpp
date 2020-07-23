#include <bits/stdc++.h>

using namespace std;

// *****

#define LEN 1'000'000'000LL

auto solve() {
    string cmd;
    cin >> cmd;
    int i = 0, len = cmd.size();
    long row = 0, col = 0;

    vector<long> multipliers = {1};
    long mul = 0;

    while (i < len) {
        long delta = multipliers.back();

        switch (cmd[i]) {
        case 'N':
            col = (col - delta + LEN) % LEN;
            break;
        case 'S':
            col = (col + delta + LEN) % LEN;
            break;
        case 'W':
            row = (row - delta + LEN) % LEN;
            break;
        case 'E':
            row = (row + delta + LEN) % LEN;
            break;
        case '(':
            multipliers.push_back((mul * delta) % LEN);
            mul = 0;
            break;
        case ')':
            multipliers.pop_back();
            break;
        default:
            mul = (mul * 10) + (cmd[i] - '0');
            break;
        }

        ++i;
    }

    return pair<long, long>{++row, ++col};
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto move = solve();
        cout << "Case #" << t << ": " << move.first << ' ' << move.second << '\n';
    }
    return 0;
}
