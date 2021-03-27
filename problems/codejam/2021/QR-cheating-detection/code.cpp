#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int P = 100, Q = 10000;
constexpr double eps = 1e-5;
int table[P][Q];
int player[P];        // number of 1s for each player
int question[Q];      // number of 1s for each question
double skill[P];      // computed skill of player
double difficulty[Q]; // computed difficulty of question

double eval(double x) { return 1.0 / (1.0 + exp(-x)); }
double integral(double x0, double x1) {
    return (log(1 + exp(-x1)) - log(1 + exp(-x0))) / (x1 - x0) + 1;
}
double performance(double S) { return integral(S - 3.0, S + 3.0); }

auto solve() {
    memset(table, 0, sizeof(table));
    memset(player, 0, sizeof(player));
    memset(question, 0, sizeof(question));

    for (int n = 0; n < P; n++) {
        string line;
        getline(cin, line);
        for (int q = 0; q < Q; q++) {
            table[n][q] = line[q] == '1';
            player[n] += table[n][q];
            question[q] += table[n][q];
        }
    }

    for (int q = 0; q < Q; q++) {
        double lo = -3.0, hi = 3.0;
        while (hi - lo >= eps) {
            double D = .5 * (lo + hi);
            if (P * performance(-D) > question[q]) {
                lo = D; // expected solves too high, increase difficulty
            } else {
                hi = D; // expected solves too low, decrease difficulty
            }
        }
        difficulty[q] = .5 * (lo + hi);
    }

    for (int n = 0; n < P; n++) {
        double lo = -4.5, hi = 4.5;
        while (hi - lo >= eps) {
            double S = .5 * (lo + hi);
            if (Q * performance(S) > player[n]) {
                hi = S; // expected solves too high, decrease skill
            } else {
                lo = S; // expected solves too low, increase skill
            }
        }
        skill[n] = .5 * (lo + hi);
    }

    int cheater = -1;
    double cheater_penalty = 0;

    for (int n = 0; n < P; n++) {
        if (player[n] < Q / 2)
            continue;
        double S = skill[n];
        double penalty = S > 3.0 ? Q * (S - 3.0) * (S - 3.0) : 0;
        for (int q = 0; q < Q; q++) {
            double D = difficulty[q], p = eval(S - D);
            penalty += table[n][q] ? 1 / p //
                                   : 1 / (1 - p);
        }
        if (penalty > cheater_penalty) {
            cheater = n;
            cheater_penalty = penalty;
        }
    }

    assert(cheater != -1);
    return cheater + 1;
}

// *****

int main() {
    unsigned T, p;
    cin >> T >> p >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
