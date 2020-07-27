#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 50'500

int N;
int E[MAXN];
int O[MAXN];

auto solve() {
    cin >> N;
    int o = 0, e = 0;
    for (int i = 0; i < N; i++) {
        if (i & 1) {
            cin >> O[o++];
        } else {
            cin >> E[e++];
        }
    }
    sort(O, O + o);
    sort(E, E + e);
    o = 0, e = 0;
    for (int i = 0; i + 1 < N; i++) {
        if (i & 1) {
            if (O[o++] > E[e]) {
                return to_string(i);
            }
        } else {
            if (E[e++] > O[o]) {
                return to_string(i);
            }
        }
    }
    return "OK"s;
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
