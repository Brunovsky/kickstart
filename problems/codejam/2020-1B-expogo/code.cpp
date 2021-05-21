#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long X, Y;
    cin >> X >> Y >> ws;
    bool xneg = X < 0;
    bool yneg = Y < 0;
    X = abs(X);
    Y = abs(Y);

    string ans;

    long P = 1;
    while (2 * P <= X + Y)
        P <<= 1;

    while (P) {
        while ((X & P) | (Y & P)) {
            assert((X & P) ^ (Y & P));
            if (X & P) {
                ans += xneg ? 'W' : 'E';
                X -= P;
                P >>= 1;
            } else {
                ans += yneg ? 'S' : 'N';
                Y -= P;
                P >>= 1;
            }
        }
        if (P == 0) {
            break;
        }
        if (X + Y >= 2 * P) {
            return "IMPOSSIBLE"s;
        }
        if (X > Y) {
            X = P - X;
            ans += xneg ? 'W' : 'E';
            xneg = !xneg;
            P >>= 1;
        } else if (X < Y) {
            Y = P - Y;
            ans += yneg ? 'S' : 'N';
            yneg = !yneg;
            P >>= 1;
        } else {
            return "IMPOSSIBLE"s;
        }
    }

    if (X != 0 || Y != 0) {
        return "IMPOSSIBLE"s;
    }

    reverse(begin(ans), end(ans));
    return ans;
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
