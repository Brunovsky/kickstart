#include <bits/stdc++.h>

using namespace std;

// *****

using Mask = bitset<100>;
struct CacheMask {
    Mask mask;
    int complaints = 0;
};

bool operator<(const CacheMask &lhs, const CacheMask &rhs) {
    int L = lhs.complaints, R = rhs.complaints;
    return L < R;
}

auto solve() {
    int N, M, P;
    cin >> N >> M >> P >> ws;
    vector<Mask> prefs(N);
    vector<Mask> bans(M);
    for (int i = 0; i < N; i++) {
        string strpref;
        getline(cin, strpref);
        prefs[i] = Mask(strpref);
    }
    for (int i = 0; i < M; i++) {
        string strban;
        getline(cin, strban);
        bans[i] = Mask(strban);
    }
    vector<int> bits(P, 0);
    for (int i = 0; i < N; i++) {
        for (int b = 0; b < P; b++) {
            bits[b] += prefs[i][b];
        }
    }

    vector<CacheMask> milks; // find M + 1 best milks
    milks.push_back({{}, 0});
    for (int b = 0; b < P; b++) {
        milks[0].complaints += bits[b];
    }

    for (int b = 0; b < P; b++) {
        int S = milks.size();
        milks.resize(2 * S);
        for (int i = 0; i < S; i++) {
            milks[i + S] = milks[i];
            milks[i].mask[b] = true;
            milks[i].complaints += N - 2 * bits[b];
        }
        sort(milks.begin(), milks.end());
        milks.resize(min(2 * S, M + 1));
    }

    int S = milks.size();
    for (int i = 0; i < S; i++) {
        if (find(begin(bans), end(bans), milks[i].mask) == end(bans)) {
            return milks[i].complaints;
        }
    }

    __builtin_unreachable();
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
