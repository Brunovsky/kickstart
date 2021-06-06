#include <bits/stdc++.h>

using namespace std;

// *****

inline bool xor_odd(uint n) {
    return __builtin_popcount(n) & 1;
}

inline bool xor_even(uint n) {
    return !xor_odd(n);
}

#define MAXN 100'000
#define MAXQ 100'000

uint N, Q, M;
uint A[MAXN];
uint P[MAXQ];
uint V[MAXQ];

auto solve() {
    cin >> N >> Q >> ws;
    for (uint n = 0; n < N; ++n) {
        cin >> A[n] >> ws;
    }
    for (uint q = 0; q < Q; ++q) {
        cin >> P[q] >> V[q] >> ws;
    }

    set<uint> odds;
    for (uint i = 0; i < N; ++i) {
        if (xor_odd(A[i])) {
            odds.insert(i);
        }
    }

    vector<uint> answer;

    for (uint q = 0; q < Q; ++q) {
        uint i = P[q];
        uint v = V[q];
        if (xor_odd(A[i]) && xor_even(v)) {
            odds.erase(i);
        } else if (xor_even(A[i]) && xor_odd(v)) {
            odds.insert(i);
        }
        A[i] = v;

        if (odds.size() & 1UL) {
            uint left = *odds.rbegin();
            uint right = N - *odds.begin() - 1;
            answer.push_back(max(left, right));
        } else {
            answer.push_back(N);
        }
    }

    return answer;
}

// *****

int main() {
    uint T;
    cin >> T >> ws;
    for (uint t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ":";
        for (uint n : solution) {
            cout << ' ' << n;
        }
        cout << '\n';
    }
    return 0;
}
