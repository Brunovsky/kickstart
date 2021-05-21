#include <bits/stdc++.h>

using namespace std;

// *****

#define P(c)           (100.0 * (c) / N)
#define R(c)           int(round(P(c)))
#define D(c)           (R(c) - P(c))
#define ROUNDS_DOWN(c) (D(c) <= 0)
#define ROUNDS_UP(c)   (D(c) > 0)

struct Lang {
    int C = 0; // number of votes
    int A = 0; // extra votes needed to round up
};

auto solve() {
    int N, L;
    int M = 0; // number of votes cast so far
    int S = 0; // sum of rounds so far
    cin >> N >> L;
    vector<Lang> langs(L);
    for (auto& l : langs) {
        cin >> l.C;
        M += l.C;
    }

    int K = 0; // number of votes needed for a new language to round up
    while (K <= N && ROUNDS_DOWN(K)) {
        K++;
    }

    // how many votes does each language need to round up?
    for (auto& [C, A] : langs) {
        while (A <= K && ROUNDS_DOWN(C + A)) {
            A++;
        }
        // A > K means creating a new language is always better
        // A = 0 already rounds up
    }
    // least votes earliest
    sort(begin(langs), end(langs), [N](Lang a, Lang b) { return a.A < b.A; });
    // add votes to existing languages, skip those that need too many
    for (auto& [C, A] : langs) {
        if (A <= K && M + A <= N) {
            M += A;
            C += A;
            A = 0;
        }
        S += R(C);
    }
    // add votes to new languages
    while (M + K <= N) {
        M += K;
        S += R(K);
    }
    // add remaining votes
    S += R(N - M);
    return S;
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
