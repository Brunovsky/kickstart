#include <bits/stdc++.h>

using namespace std;

// *****

int R, C;
int H[50][50];
int W[50][50];
bool F[50][50];

struct point {
    int r, c;
};

bool operator<(point a, point b) {
    return tie(W[a.r][a.c], a.r, a.c) < tie(W[b.r][b.c], b.r, b.c);
}

bool is_sea(point p) {
    return p.r < 0 || p.c < 0 || p.r >= R || p.c >= C;
}

constexpr point dd[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

auto solve() {
    memset(H, 0, sizeof(H));
    memset(W, 0, sizeof(W));
    memset(F, 0, sizeof(F));
    cin >> R >> C >> ws;
    if (R <= 2 || C <= 2) {
        return 0;
    }
    assert(R <= 50 && C <= 50);

    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            cin >> H[r][c];
        }
    }
    memcpy(W, H, sizeof(W));

    set<point> valleys;

    for (int r = 0; r < R; ++r) {
        valleys.insert({r, 0});
        valleys.insert({r, C - 1});
        F[r][0] = F[r][C - 1] = true;
    }
    for (int c = 0; c < C; ++c) {
        valleys.insert({0, c});
        valleys.insert({R - 1, c});
        F[0][c] = F[R - 1][c] = true;
    }

    int water = 0;
    while (!valleys.empty()) {
        point V = *valleys.begin();
        valleys.erase(valleys.begin());
        F[V.r][V.c] = true;
        int hV = W[V.r][V.c];

        for (point d : dd) {
            point N = {V.r + d.r, V.c + d.c};
            if (is_sea(N) || F[N.r][N.c]) {
                continue;
            }
            int hN = W[N.r][N.c];
            if (hN < hV) {
                water += hV - hN;
            }
            valleys.erase(N);
            W[N.r][N.c] = max(W[N.r][N.c], hV);
            valleys.insert(N);
        }
    }
    return water;
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
