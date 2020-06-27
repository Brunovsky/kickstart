#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N >> ws;
    int best_num = -1;
    string best;

    for (int i = 0; i < N; ++i) {
        string candidate;
        getline(cin, candidate);
        bitset<26> alph;
        for (char c : candidate)
            if (c != ' ')
                alph[c - 'A'] = true;
        int candidate_num = alph.count();
        if (candidate_num > best_num) {
            best = move(candidate);
            best_num = candidate_num;
        } else if (candidate_num == best_num && candidate < best) {
            best = move(candidate);
        }
    }

    return best;
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
