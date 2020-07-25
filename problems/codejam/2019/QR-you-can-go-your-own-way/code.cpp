#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N >> ws;
    string line;
    getline(cin, line);
    for (int i = 0; i < 2 * N - 2; i++) {
        line[i] = line[i] == 'S' ? 'E' : 'S';
    }
    return line;
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
