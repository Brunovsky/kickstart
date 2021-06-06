#include <bits/stdc++.h>

using namespace std;

// *****

long choose(long n, const vector<long>& k) {
    assert(accumulate(begin(k), end(k), 0L) <= n);
    long multi = 1, m = 1, r = 1;
    for (int i = 0, K = k.size(); i < K; i++) {
        for (int j = 1; j <= k[i]; j++)
            multi = multi * m++ / j;
    }
    while (m < n)
        multi = multi * m++ / r++;
    return multi;
}

int sum(const string& s) {
    int n = 0;
    for (char c : s)
        n += c - '0';
    return n;
}

int branch_size(const string& s) {
    int S = s.size();
    if (sum(s) > S)
        return 1;

    string pred(S, '0');
    for (int i = 0, k = S - sum(s); i < S; i++)
        for (int j = 0; j < s[i] - '0'; j++, k++)
            pred[k] = '1' + i;

    if (sum(pred) > S) {
        vector<long> k(S + 1, 0);
        for (char c : pred)
            k[c - '0']++;
        return 1 + choose(S, k);
    }

    int n = 1;
    do {
        if (s != pred)
            n += branch_size(pred);
    } while (next_permutation(begin(pred), end(pred)));
    return n;
}

auto solve() {
    string G;
    getline(cin, G);
    return branch_size(G);
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
