#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXS 220
#define S    110

int score = 0;
bool row[MAXS], col[MAXS], sum[MAXS], dif[MAXS];
bool hasp[S][S], hasx[S][S], modified[S][S];

bool can_p(int x, int y) { return !sum[x + y] && !dif[x - y + S]; }
bool can_x(int x, int y) { return !row[x] && !col[y]; }
bool can_o(int x, int y) { return can_p(x, y) && can_x(x, y); }
void putpiece(char c, int x, int y) {
    if (c == '+' || c == 'o')
        assert(can_p(x, y)), sum[x + y] = dif[x - y + S] = hasp[x][y] = true, score++;
    if (c == 'x' || c == 'o')
        assert(can_x(x, y)), row[x] = col[y] = hasx[x][y] = true, score++;
}
void modify(char c, int x, int y) { putpiece(c, x, y), modified[x][y] = true; }

auto solve() {
    memset(row, 0, sizeof(row));
    memset(col, 0, sizeof(col));
    memset(sum, 0, sizeof(sum));
    memset(dif, 0, sizeof(dif));
    memset(hasp, 0, sizeof(hasp));
    memset(hasx, 0, sizeof(hasx));
    memset(modified, 0, sizeof(modified));
    score = 0;

    int N, M;
    cin >> N >> M >> ws;
    for (int i = 0; i < M; i++) {
        int x, y;
        char c;
        cin >> c >> x >> y >> ws;
        putpiece(c, x, y);
    }

    for (int x = 1; x <= N; x++) {
        for (int y = 1; y <= N; y++) {
            if (can_x(x, y))
                modify('x', x, y);
        }
    }
    // 1+1=2   N+N=2N   mid=(2+2N)/2=N+1
    for (int d = 1; d <= N; d++) {
        for (int x = 1, y = d; y >= 1; x++, y--) {
            if (can_p(x, y))
                modify('+', x, y);
        }
        for (int x = N, y = N + 1 - d; y <= N; x--, y++) {
            if (can_p(x, y))
                modify('+', x, y);
        }
    }
    stringstream ss;
    int mods = 0;
    for (int x = 1; x <= N; x++) {
        for (int y = 1; y <= N; y++) {
            if (modified[x][y]) {
                char c = hasp[x][y] ? (hasx[x][y] ? 'o' : '+') : 'x';
                ss << c << ' ' << x << ' ' << y << '\n';
                mods++;
            }
        }
    }
    cout << score << ' ' << mods << '\n' << ss.str();
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
