#include <bits/stdc++.h>

using namespace std;

// *****

int E, N;
vector<int> S;
vector<int> P;

auto solve() {
    cin >> E >> N >> ws;
    S.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> S[i];
    }
    sort(begin(S), end(S));
    P.resize(N + 1, 0);
    for (int i = 0; i < N; i++) {
        P[i + 1] = P[i] + S[i];
    }

    if (E <= S[0]) {
        return 0;
    }

    // define three ranges [0,L), [L,R), [R,N)
    // defeat the [0,L) morons (+L honor, -P[L] energy)
    // truce the [L,R) morons
    // recruit the [R,N) morons (-(N-R) honor, P[N] - P[R] energy)
    int honor = 0;
    for (int L = 0; L <= N; L++) {
        for (int R = L; R <= N; R++) {
            int defeat_energy = P[L];
            int recruit_energy = P[N] - P[R];
            if (recruit_energy + E > defeat_energy) {
                honor = max(honor, L + R - N);
            }
        }
    }
    return honor;
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
