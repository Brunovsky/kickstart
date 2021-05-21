#include <bits/stdc++.h>

using namespace std;

// *****

string expand(string s) {
    string e;
    e.reserve(4 * s.size());
    for (char c : s) {
        e.append(c == '*' ? 4 : 1, c);
    }
    return e;
}

auto solve() {
    string a, b;

    getline(cin, a);
    getline(cin, b);

    a = expand(a);
    b = expand(b);

    int A = a.size(), B = b.size();

    vector<bool> prev(8001, false);
    vector<bool> next(8001, false);
    next[0] = true;

    for (int j = 0; j < B; ++j) {
        if (b[j] != '*') {
            break;
        }
        next[j + 1] = true;
    }

    for (int i = 0; i < A; ++i) {
        swap(prev, next);
        next[0] = a[i] == '*' && prev[0];

        for (int j = 0; j < B; ++j) {
            if (a[i] == '*' && b[j] == '*') {
                next[j + 1] = next[j] || prev[j] || prev[j + 1];
            } else if (a[i] == '*') {
                next[j + 1] = prev[j] || prev[j + 1];
            } else if (b[j] == '*') {
                next[j + 1] = prev[j] || next[j];
            } else {
                next[j + 1] = a[i] == b[j] && prev[j];
            }
        }
    }

    return next[B] ? "TRUE" : "FALSE";
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
