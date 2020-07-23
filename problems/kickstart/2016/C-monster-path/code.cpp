#include <bits/stdc++.h>

using namespace std;

// *****

#define MAX 20

enum dir_t : int { NORTH = 0, WEST = 1, SOUTH = 2, EAST = 3 };

int R, C, Rs, Cs, S;
double P, Q;
double original[MAX][MAX];
double encounter[MAX][MAX];
vector<dir_t> dirs;

void move(int &r, int &c, dir_t dir) {
    if (dir == NORTH) {
        --r;
    } else if (dir == WEST) {
        --c;
    } else if (dir == SOUTH) {
        ++r;
    } else if (dir == EAST) {
        ++c;
    }
}

bool check_path() {
    int r = Rs, c = Cs;
    for (auto dir : dirs) {
        move(r, c, dir);
        if (r < 0 || c < 0 || r >= R || c >= C) {
            return false;
        }
    }
    return true;
}

void restore_path() {
    int r = Rs, c = Cs;
    for (auto dir : dirs) {
        move(r, c, dir);
        encounter[r][c] = original[r][c];
    }
}

double compute_path() {
    int r = Rs, c = Cs;
    double p = 0.0;
    for (auto dir : dirs) {
        move(r, c, dir);
        p += encounter[r][c];
        encounter[r][c] *= 1.0 - original[r][c];
    }
    return p;
}

auto solve() {
    cin >> R >> C >> Rs >> Cs >> S >> ws >> P >> Q >> ws;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            char cell;
            cin >> cell >> ws;
            encounter[r][c] = original[r][c] = cell == 'A' ? P : Q;
        }
    }
    dirs.assign(S, NORTH);
    double best = 0.0;
    int gen = 0, max_gen = 1 << (2 * S);
    do {
        int bits = gen, i = 0;
        while (bits > 0) {
            auto dir = dir_t(bits & 3);
            bits >>= 2;
            dirs[i++] = dir;
        }
        if (check_path()) {
            double p = compute_path();
            best = max(best, p);
            restore_path();
        }
    } while (++gen < max_gen);

    return best;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    cout << showpoint << fixed << setprecision(9);
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
