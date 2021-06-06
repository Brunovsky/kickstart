#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    string s;
    getline(cin, s);
    int S = s.size();
    vector<int> kick, start;
    for (int i = 0; i < S; i++) {
        if (s.substr(i, 4) == "KICK")
            kick.push_back(i);
        if (s.substr(i, 5) == "START")
            start.push_back(i);
    }
    int A = kick.size(), B = start.size();
    int b = 0;
    long answers = 0;
    for (int a = 0; a < A && b < B; a++) {
        while (b < B && start[b] < kick[a])
            b++;
        answers += B - b;
    }
    return answers;
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
