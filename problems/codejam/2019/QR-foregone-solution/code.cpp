#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string N;
    getline(cin, N);
    int s = N.size();
    string a = N;
    string b(s, '0');
    for (int i = 0; i < s; i++) {
        if (N[i] == '4') {
            a[i] = b[i] = '2';
        }
    }
    b.erase(0, b.find_first_not_of('0'));
    return a + ' ' + b;
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
