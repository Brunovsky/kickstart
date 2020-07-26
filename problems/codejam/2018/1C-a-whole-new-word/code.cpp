#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 2000

int N, L;
string W[MAXN];
vector<set<char>> chars;

auto solve() {
    cin >> N >> L;
    chars.assign(L, {});
    for (int i = 0; i < N; i++) {
        cin >> W[i];
    }
    if (L == 1) {
        return "-"s;
    }
    sort(W, W + N);
    for (int p = 0; p < L; p++) {
        for (int i = 0; i < N; i++) {
            chars[p].insert(W[i][p]);
        }
    }
    size_t cnt = 1;
    for (int p = 0; p < L; p++) {
        cnt *= chars[p].size();
    }
    if (cnt <= size_t(N)) {
        return "-"s;
    }
    vector<string> answers{""};
    for (int p = L - 1; p >= 0; p--) {
        int S = answers.size();
        if (S > N) {
            break;
        }
        int NS = S * chars[p].size();
        answers.resize(NS);
        for (int i = S; i < NS; i++) {
            answers[i] = answers[i - S];
        }
        int j = 0;
        for (char c : chars[p]) {
            for (int i = 0; i < S; i++, j++) {
                answers[j] += c;
            }
        }
    }
    int S = answers.size();
    int k = answers[0].size();
    for (int p = L - 1 - k; p >= 0; p--) {
        for (int i = 0; i < S; i++) {
            answers[i] += *chars[p].begin();
        }
    }
    for (int i = 0; i < S; i++) {
        reverse(begin(answers[i]), end(answers[i]));
    }
    for (int i = 0; i < N; i++) {
        assert(answers[i] <= W[i]);
        if (answers[i] != W[i]) {
            return answers[i];
        }
    }
    return answers[N];
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
