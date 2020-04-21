#include <bits/stdc++.h>

using namespace std;

// *****

#define LEN 1'000'000'000ll

struct Move {
    long long row, col;
};

Move parse(const string &cmd) {
    int i = 0, len = cmd.size();
    long long row = 0, col = 0;

    vector<long long> mul_stack = {1};
    long long mul = 0;

    while (i < len) {
        long long delta = mul_stack.back();

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
            mul_stack.push_back((mul * delta) % LEN);
            mul = 0;
            break;
        case ')':
            mul_stack.pop_back();
            break;
        default:
            mul = (mul * 10) + (cmd[i] - '0');
            break;
        }

        ++i;
    }

    return {++row, ++col};
}

auto solve() {
    string cmd;
    cin >> cmd;
    return parse(cmd);
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto move = solve();
        cout << "Case #" << t << ": " << move.row << ' ' << move.col << '\n';
    }
    return 0;
}
