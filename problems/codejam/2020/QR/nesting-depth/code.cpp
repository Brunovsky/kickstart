#include <bits/stdc++.h>

using namespace std;

string split(string S, size_t n) {
    if (S.empty() || n >= 10)
        return "";

    size_t i = 0, e = 0;
    string solution;
    solution.reserve(S.size() << 1);

    while (i < S.size()) {
        while (e < S.size() && S[e] != char('0' + n))
            ++e;
        if (e > i)
            solution += "(" + split(S.substr(i, e - i), n + 1) + ")";
        if (e < S.size())
            solution += '0' + n;

        i = ++e;
    }
    return solution;
}

// *****

auto solve() {
    string line;
    cin >> line;
    return split(line, 0);
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
