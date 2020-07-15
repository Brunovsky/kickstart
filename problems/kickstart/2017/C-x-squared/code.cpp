#include <bits/stdc++.h>

using namespace std;

// *****

int N;
vector<string> grid;

auto solve() {
    cin >> N >> ws;
    grid.resize(N);
    for (int i = 0; i < N; i++) {
        getline(cin, grid[i]);
    }

    int r_one, c_one;

    for (int r = 0; r < N; r++) {
        int count = 0;
        for (int c = 0; c < N; c++) {
            count += grid[r][c] == 'X';
        }
        if (count > 2 || count == 0) {
            return "IMPOSSIBLE";
        }
        if (count == 1) {
            r_one = r;
        }
    }

    for (int c = 0; c < N; c++) {
        int count = 0;
        for (int r = 0; r < N; r++) {
            count += grid[r][c] == 'X';
        }
        if (count > 2 || count == 0) {
            return "IMPOSSIBLE";
        }
        if (count == 1) {
            c_one = c;
        }
    }

    // this is not actually necessary
    if (grid[r_one][c_one] != 'X') {
        return "IMPOSSIBLE";
    }

    for (int r = 0; r < N; r++) {
        if (r == r_one) {
            continue;
        }
        for (int c = 0; c < N; c++) {
            if (grid[r][c] == 'X' && c != c_one) {
                int r2 = r;
                int c2 = c;
                do {
                    ++r2;
                } while (r2 < N && grid[r2][c] != 'X');
                do {
                    ++c2;
                } while (c2 < N && grid[r][c2] != 'X');
                if (r2 == N || c2 == N || grid[r2][c2] != 'X') {
                    return "IMPOSSIBLE";
                }
                grid[r][c] = grid[r][c2] = grid[r2][c] = grid[r2][c2] = '.';
                break;
            }
        }
    }

    return "POSSIBLE";
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
