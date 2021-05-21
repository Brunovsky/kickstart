#include <bits/stdc++.h>

using namespace std;

// *****

vector<string> grid;

void adjust(int i, int j, char& c) {
    if (grid[i][j] == '?') {
        grid[i][j] = c;
    } else {
        c = grid[i][j];
    }
}

auto solve() {
    int R, C;
    cin >> R >> C;
    grid.resize(R);
    for (int i = 0; i < R; i++) {
        cin >> grid[i];
    }
    for (int i = 0; i < R; i++) {
        char c = '?';
        for (int j = 0; j < C; j++)
            adjust(i, j, c);
    }
    for (int i = 0; i < R; i++) {
        char c = '?';
        for (int j = C - 1; j >= 0; j--)
            adjust(i, j, c);
    }
    for (int j = 0; j < C; j++) {
        char c = '?';
        for (int i = 0; i < R; i++)
            adjust(i, j, c);
    }
    for (int j = 0; j < C; j++) {
        char c = '?';
        for (int i = R - 1; i >= 0; i--)
            adjust(i, j, c);
    }

    for (int i = 0; i < R; i++) {
        cout << grid[i] << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":\n";
        solve();
    }
    return 0;
}
