#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 50
#define PREFIX(str, n) str.substr(0, n)
#define SUFFIX(str, n) str.substr(str.size() - n, n)

int N;
string patterns[MAXN];
int starl[MAXN], starr[MAXN];

auto solve() {
    cin >> N >> ws;
    for (int i = 0; i < N; i++) {
        cin >> patterns[i] >> ws;
        int S = patterns[i].size();
        for (int j = 0; j < S; j++) {
            if (patterns[i][j] == '*') {
                starl[i] = j;
                break;
            }
        }
        for (int j = S - 1; j >= 0; j--) {
            if (patterns[i][j] == '*') {
                starr[i] = j;
                break;
            }
        }
    }
    string a, m, b;
    int A = 0, M = 0, B = 0;
    for (int i = 0; i < N; i++) {
        const auto &p = patterns[i];
        int S = p.size();
        int sl = starl[i], sr = S - starr[i] - 1;
        int left = min(sl, A), right = min(sr, B);
        if (left && PREFIX(p, left) != PREFIX(a, left)) {
            return "*"s;
        }
        if (right && SUFFIX(p, right) != SUFFIX(b, right)) {
            return "*"s;
        }
        if (sl > A) {
            a = PREFIX(p, sl);
            A = sl;
        }
        if (sr > B) {
            b = SUFFIX(p, sr);
            B = sr;
        }
        for (int k = starl[i] + 1; k < starr[i]; k++) {
            if (p[k] != '*') {
                m += p[k];
            }
        }
    }
    return a + m + b;
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
