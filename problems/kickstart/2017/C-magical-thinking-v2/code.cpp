#include <bits/stdc++.h>

using namespace std;

// N = 2
// ===== Don't know what's wrong :(
// suppose d1 out of D1 are correct answers (for A1 and B)
//     D1 - d1 are correct answers for A2
// suppose d2 out of D2 are correct answers (for A2 and B)
//     D2 - d2 are correct answers for A1
// suppose e1 out of E1 are correct answers (for B)
//     E1 - e1 are correct answers for A1, A2
// suppose e2 out of E2 are correct answers (for A1, A2 and B)
// then:
// correct answers for A1:  d1 - d2 - e1 + e2 + E1 + D2  (must equal S1)
// correct answers for A2:  d2 - d1 - e1 + e2 + E1 + D1  (must equal S2)
// correct answers for B:   d1 + d2 + e1 + e2            (we must maximize)
//
// formulation:
//   max  d1 + d2 + e2 + e1
//   subject to
//        d1 - d2  + e2 - e1 = S1 - D2 - E1
//      -(d1 - d2) + e2 - e1 = S2 - D1 - E1
//        0 <= d1 <= D1
//        0 <= d2 <= D2
//        0 <= e1 <= E1
//        0 <= e2 <= E2
//
// solution:
// the possible range of d1 - d2 is [-D2,D1] and d1 + d2 is [0,D1+D2] so replace
//   dd := d1 - d2 (difference of ds)
//   sd := d1 + d2 (sum of d)
// the possible range of e1 - e2 is [-E1,E2] and e1 + e2 is [0,E1+E2] so replace
//   de := e1 - e2 (difference of es)
//   se := e1 + e2 (sum of e)
//
// reformulation:
//   max  sd + se
//   subject to
//      -de + dd = S1 - D2 - E1
//      -de - dd = S2 - D1 - E1
//      -D2 <= dd <= D1
//        0 <= sd <= D1 + D2
//      -E2 <= de <= E1
//        0 <= se <= E1 + E2
//
// solve for dd and de:
//   dd := (S1 + D1 - S2 - D2) / 2
//   de := (2 * E1 + D1 + D2 - S1 - S2) / 2
//
// to finish (se and sd) apply the fact that the extrema of se (resp. sd) will
// be reached when one of the terms is maximized - e1 or e2 (resp. d1 or d2).

// *****

int N, Q;
string A1s, A2s, Bs;

auto get(const string &line) {
    bitset<50> mask;
    for (int i = 0; i < Q; i++) {
        mask[i] = line[i] == 'T';
    }
    return mask;
}

auto solve1() {
    getline(cin, A1s);
    auto A = get(A1s);
    getline(cin, Bs);
    auto B = get(Bs);

    int S;
    cin >> S >> ws;

    int C1 = (A ^ B).count();
    int C0 = Q - C1;

    return Q - abs(S - C0);
}

auto solve2() {
    getline(cin, A1s);
    auto A1 = get(A1s);
    getline(cin, A2s);
    auto A2 = get(A2s);
    getline(cin, Bs);
    auto B = get(Bs);

    int S1, S2;
    cin >> S1 >> S2 >> ws;

    // patterns are bits for A1A2B.

    // A1 and A2 gave different answers
    // ... and B matches A1
    int D1 = ((~A1 & A2 & ~B) | (A1 & ~A2 & B)).count(); // 010, 101
    // ... and B matches A2
    int D2 = ((~A1 & A2 & B) | (A1 & ~A2 & ~B)).count(); // 011, 100

    // A1 and A2 gave the same answers
    // ... and B gave a different answer
    int E1 = ((~A1 & ~A2 & B) | (A1 & A2 & ~B)).count(); // 001, 110
    // ... and B gave the same answer too
    int E2 = Q - D1 - D2 - E1; // 000, 111

    // magical thinking indeed:

    int dd = ((S1 + D1) - (S2 + D2)) / 2;   // d1 - d2
    int sd = min(2 * D1 - dd, 2 * D2 + dd); // d1 + d2

    int de = (2 * E1 + (D1 + D2) - (S1 + S2)) / 2; // e1 - e2
    int se = min(2 * E1 - de, 2 * E2 + de);        // e1 + e2

    return sd + se;
}

auto solve() {
    cin >> N >> Q >> ws;
    return N == 1 ? solve1() : solve2();
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
