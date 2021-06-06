#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXQ 100'001

int P, Q;
int Xcum[MAXQ];
int Ycum[MAXQ];

auto solve() {
    cin >> P >> Q >> ws;
    memset(Xcum, 0, sizeof(Xcum));
    memset(Ycum, 0, sizeof(Ycum));
    for (int i = 0; i < P; i++) {
        int X, Y;
        char D;
        cin >> X >> Y >> ws >> D >> ws;
        if (D == 'N') {
            Ycum[Q]++;
            Ycum[Y]--;
        }
        if (D == 'S') {
            Ycum[Y - 1]++;
        }
        if (D == 'E') {
            Xcum[Q]++;
            Xcum[X]--;
        }
        if (D == 'W') {
            Xcum[X - 1]++;
        }
    }
    for (int i = Q - 1; i >= 0; i--) {
        Xcum[i] += Xcum[i + 1];
        Ycum[i] += Ycum[i + 1];
    }
    int x = max_element(Xcum, Xcum + Q + 1) - Xcum;
    int y = max_element(Ycum, Ycum + Q + 1) - Ycum;
    return to_string(x) + ' ' + to_string(y);
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
