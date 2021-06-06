#include <bits/stdc++.h>

using namespace std;

// *****

#define LINES 10'000
#define MAXU 19

int U;
int freq[128][MAXU];
double score[128];

bool cmp_digits(int c, int d) {
    return !freq[c][U] || (freq[d][U] && score[c] > score[d]);
}

auto solve() {
    memset(freq, 0, sizeof(freq));
    memset(score, 0, sizeof(score));
    string str;

    cin >> U >> ws;
    for (int line = 0; line < LINES; line++) {
        long Q;
        string R;
        cin >> Q >> ws >> R >> ws;
        int L = R.size();
        assert(L <= U);

        long n = 1;
        for (int i = L - 1; i >= 0; i--) {
            uint c = R[i];
            freq[c][L - i]++;
            if (str.find(c) == string::npos) {
                str.push_back(c);
            }
            if (Q == -1) {
                score[c] += n;
            } else {
                // Benford's law for all digits :)
                score[c] += 1.0 * n / (Q - n + 1);
            }
            n *= 10;
        }
    }

    sort(begin(str), end(str), cmp_digits);
    for (uint c : str) {
        fprintf(stderr, "%c: %4d  %.1lf\n", char(c), freq[c][U], score[c]);
    }

    return str;
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
