#include <bits/stdc++.h>

using namespace std;

// *****

//   X X _ X X _ X X _ ....
//   _ X X _ X X _ X X ....
//   X _ X X _ X X _ X ....
//   . . . . . . . . . .
//   . . . . . . . . .   .
// when 2 rows or 2 columns:
//   X X _ X X _ X X _ ....
//   X X _ X X _ X X _ ....

auto solve() {
    int R, C;
    cin >> R >> C;
    if (R < C) {
        swap(R, C);
    }
    // there are probably cleaner ways to compute the number of Xs here...
    int r2 = (2 * R + 2) / 3;
    int r1 = (2 * R + 1) / 3;
    int r0 = (2 * R + 0) / 3;
    int c2 = (C + 2) / 3;
    int c1 = (C + 1) / 3;
    int c0 = (C + 0) / 3;
    if (C <= 2) {
        return C * r2;
    }
    return r2 * c2 + r1 * c1 + r0 * c0;
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
