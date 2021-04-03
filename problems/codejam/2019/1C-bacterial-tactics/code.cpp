#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXS 17

int R, C;
int virus[MAXS][MAXS];
int nim[MAXS][MAXS][MAXS][MAXS];

int bad(int ra, int rb, int ca, int cb) {
    return virus[rb][cb] - virus[ra - 1][cb] - virus[rb][ca - 1] + virus[ra - 1][ca - 1];
}

int mex(vector<int>& nims) {
    sort(begin(nims), end(nims));
    int i = 0, n = 0, S = nims.size();
    while (i < S && nims[i] <= n)
        n = nims[i++] + 1;
    return n;
}

auto solve() {
    cin >> R >> C;
    memset(virus, 0, sizeof(virus));
    memset(nim, 0, sizeof(nim));
    for (int r = 1; r <= R; r++) {
        string line;
        cin >> line;
        for (int c = 1; c <= C; c++) {
            virus[r][c] = line[c - 1] == '#';
        }
    }

    for (int r = 1; r <= R; r++)
        for (int c = 1; c <= C; c++)
            virus[r][c] += virus[r - 1][c];
    for (int r = 1; r <= R; r++)
        for (int c = 1; c <= C; c++)
            virus[r][c] += virus[r][c - 1];

    for (int dr = 0; dr < R; dr++) {
        for (int ra = 1, rb = 1 + dr; rb <= R; ra++, rb++) {
            for (int dc = 0; dc < C; dc++) {
                for (int ca = 1, cb = 1 + dc; cb <= C; ca++, cb++) {
                    vector<int> nims;
                    for (int r = ra; r <= rb; r++) {
                        if (!bad(r, r, ca, cb)) {
                            int n = nim[ra][r - 1][ca][cb] ^ nim[r + 1][rb][ca][cb];
                            nims.push_back(n);
                        }
                    }
                    for (int c = ca; c <= cb; c++) {
                        if (!bad(ra, rb, c, c)) {
                            int n = nim[ra][rb][ca][c - 1] ^ nim[ra][rb][c + 1][cb];
                            nims.push_back(n);
                        }
                    }
                    nim[ra][rb][ca][cb] = mex(nims);
                }
            }
        }
    }

    if (nim[1][R][1][C] == 0)
        return 0;

    int moves = 0;
    for (int r = 1; r <= R; r++) {
        if (!bad(r, r, 1, C) && (nim[1][r - 1][1][C] ^ nim[r + 1][R][1][C]) == 0)
            moves += C;
    }
    for (int c = 1; c <= C; c++) {
        if (!bad(1, R, c, c) && (nim[1][R][1][c - 1] ^ nim[1][R][c + 1][C]) == 0)
            moves += R;
    }
    return moves;
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
