#include <bits/stdc++.h>

using namespace std;

// *****

string make(int P, int R, int S) {
    assert(P >= 0 && R >= 0 && S >= 0);
    if (P + R + S == 1) {
        if (P == 1)
            return "P";
        if (R == 1)
            return "R";
        if (S == 1)
            return "S";
        assert(false);
    }
    int a = (P + R + S) / 2 - S; // P vs R -> P (write PR)
    int b = (P + R + S) / 2 - P; // R vs S -> R (write RS)
    int c = (P + R + S) / 2 - R; // S vs P -> S (write PS)
    if (a < 0 || b < 0 || c < 0)
        return "";

    auto s = make(a, b, c);
    if (s.empty())
        return "";

    string w;
    for (char p : s)
        if (p == 'P')
            w += "RP";
        else if (p == 'R')
            w += "SR";
        else
            w += "SP";
    return w;
}

auto get(int N, int P, int R, int S) {
    int M = P + R + S;
    assert(M == (1 << N));
    auto s = make(P, R, S);
    if (s.empty())
        return "IMPOSSIBLE"s;

    for (int n = 1; n < M; n <<= 1) {
        for (int i = 0; i < M; i += 2 * n) {
            auto a = s.substr(i, n), b = s.substr(i + n, n);
            if (a > b) {
                s.replace(i, n, b);
                s.replace(i + n, n, a);
            }
        }
    }
    return s;
}

void test() {
    for (int N = 1; N <= 3; N++) {
        for (int M = 1 << N, P = 0; P <= M; P++) {
            for (int R = 0, S = M - P - R; P + R <= M; R++, S--) {
                printf("%d %2d %2d %2d -- %s\n", N, P, R, S, get(N, P, R, S).data());
            }
        }
    }
}

auto solve() {
    int N, P, R, S;
    cin >> N >> P >> R >> S;
    return get(N, P, R, S);
}

// *****

int main() {
    // test();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
