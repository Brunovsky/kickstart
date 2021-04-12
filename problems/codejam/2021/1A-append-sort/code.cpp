#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<string> X(N);
    for (int i = 0; i < N; i++)
        cin >> ws >> X[i];
    int appends = 0;
    for (int i = 0; i + 1 < N; i++) {
        const auto& a = X[i];
        auto& b = X[i + 1];
        int n = a.size(), m = b.size();
        if (n == m) {
            if (a >= b)
                b += "0", appends++;
        } else if (n > m) {
            if (a.substr(0, m) > b) {
                b += string(n - m + 1, '0'), appends += n - m + 1;
            } else if (a.substr(0, m) < b) {
                b += string(n - m, '0'), appends += n - m;
            } else if (a.substr(m) == string(n - m, '9')) {
                b += string(n - m + 1, '0'), appends += n - m + 1;
            } else {
                auto c = to_string(stol(a.substr(m)) + 1);
                c = string(n - m - c.size(), '0') + c;
                b += c, appends += n - m;
            }
        }
    }
    return appends;
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
