#include <bits/stdc++.h>

using namespace std;

// *****

string repeat(const string& s, int n) {
    string k;
    while (n-- > 0)
        k += s;
    return k;
}

tuple<int, int, int, char, char, char> order(int R, int Y, int B) {
    if (R >= max(Y, B)) {
        return {R, Y, B, 'R', 'Y', 'B'};
    } else if (Y >= max(R, B)) {
        return {Y, B, R, 'Y', 'B', 'R'};
    } else {
        return {B, R, Y, 'B', 'R', 'Y'};
    }
}

auto solve() {
    int N, R, b, Y, r, B, y;
    cin >> N >> R >> b >> Y >> r >> B >> y;
    int nonzero = (b > 0) + (r > 0) + (y > 0) + (R > 0) + (Y > 0) + (B > 0);

    if (b > B || r > R || y > Y) {
        return "IMPOSSIBLE"s;
    }
    if (((B && b == B) || (R && r == R) || (Y && y == Y)) && nonzero > 2) {
        return "IMPOSSIBLE"s;
    }
    if (R && r == R) {
        return repeat("GR", R);
    }
    if (Y && y == Y) {
        return repeat("VY", Y);
    }
    if (B && b == B) {
        return repeat("OB", B);
    }
    R -= r, Y -= y, B -= b;
    N -= 2 * (b + r + y);

    int C = max(R, max(Y, B));
    if (2 * C > N) {
        return "IMPOSSIBLE"s;
    }
    int T = N - 2 * C;
    R -= T, Y -= T, B -= T, C -= T, N -= 3 * T;

    auto [u, v, w, mu, mv, mw] = order(R, Y, B);
    string solution = repeat({mu, mv, mw}, T);
    solution += repeat({mu, mv}, v);
    solution += repeat({mu, mw}, w);

    if (r > 0) {
        solution.replace(solution.find('R'), 1, "R" + repeat("GR", r));
    }
    if (y > 0) {
        solution.replace(solution.find('Y'), 1, "Y" + repeat("VY", y));
    }
    if (b > 0) {
        solution.replace(solution.find('B'), 1, "B" + repeat("OB", b));
    }

    return solution;
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
