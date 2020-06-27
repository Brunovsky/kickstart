#include <bits/stdc++.h>

using namespace std;

// *****

using alph = array<uint16_t, 26>;

constexpr uint64_t MOD = 1000000007;

alph sub(alph lhs, alph rhs) {
    alph word;
    for (int i = 0; i < 26; ++i) {
        word[i] = lhs[i] - rhs[i];
    }
    return word;
}

auto solve() {
    int V, S;
    cin >> V >> S >> ws;
    map<alph, uint64_t> vocab;
    for (int v = 0; v < V; ++v) {
        string word;
        getline(cin, word);
        alph word_vocab = {};
        for (char c : word)
            ++word_vocab[c - 'a'];
        vocab[word_vocab]++;
    }

    vector<uint64_t> counts;
    for (int s = 0; s < S; ++s) {
        string e;
        getline(cin, e);
        int E = e.size();

        vector<alph> ps(E + 1);
        ps[0] = {};
        for (int i = 0; i < E; ++i) {
            ps[i + 1] = ps[i];
            ++ps[i + 1][e[i] - 'a'];
        }

        vector<uint64_t> dp(E + 1, 0);
        dp[0] = 1;

        for (int j = 1; j <= E; ++j) {
            int i = max(0, j - 20);
            alph word = sub(ps[j], ps[i]);

            while (i < j) {
                if (vocab.count(word)) {
                    dp[j] = (dp[j] + vocab.at(word) * dp[i]) % MOD;
                }
                --word[e[i++] - 'a'];
            }
        }
        counts.push_back(dp[E]);
    }

    return counts;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ":";
        for (auto count : solution)
            cout << ' ' << count;
        cout << '\n';
    }
    return 0;
}
