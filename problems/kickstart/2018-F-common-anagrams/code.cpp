#include <bits/stdc++.h>

using namespace std;

// *****

#define ASCII 26

using alph = array<int, ASCII>;

alph sub(const alph &lhs, const alph &rhs) {
    alph res;
    for (int i = 0; i < ASCII; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

ulong L;
string A, B;
vector<alph> pA, pB;

auto solve() {
    cin >> L >> ws >> A >> ws >> B >> ws;
    assert(A.length() == L && B.length() == L);

    pA.assign(L + 1, {});
    pB.assign(L + 1, {});
    pA[0] = {};
    pB[0] = {};

    ulong numtuples = 0;

    for (uint i = 1; i <= L; ++i) {
        pA[i] = pA[i - 1];
        pB[i] = pB[i - 1];
        ++pA[i][A[i - 1] - 'A'];
        ++pB[i][B[i - 1] - 'A'];
    }

    for (uint len = 1; len <= L; ++len) {
        ulong num = L - len + 1;

        vector<alph> sA(num), sB(num);

        for (uint i = 0, j = len; j <= L; ++i, ++j) {
            sA[i] = sub(pA[j], pA[i]);
            sB[i] = sub(pB[j], pB[i]);
        }

        sort(begin(sA), end(sA));
        sort(begin(sB), end(sB));

        ulong a = 0, b = 0;
        while (a < num && b < num) {
            if (sA[a] == sB[b]) {
                ++numtuples;
                ++a;
            } else if (sA[a] < sB[b]) {
                ++a;
            } else {
                ++b;
            }
        }
    }

    return numtuples;
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
