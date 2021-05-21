#include <bits/stdc++.h>

using namespace std;

// *****

using intd = uniform_int_distribution<int>;
using reald = uniform_real_distribution<double>;
using boold = bernoulli_distribution;

mt19937 mt(random_device{}());
intd distP(0, 100 - 1);
reald dist3(-3.0, 3.0);
boold coin(0.5);

constexpr double eval(double x) { return 1.0 / (1.0 + exp(-x)); }

#define Q 10000
#define T 10
#define P 100

int main() {
    printf("%d\n%d\n", T, 86);
    vector<int> cheaters;
    for (int t = 1; t <= T; t++) {
        vector<double> D(Q);
        vector<double> S(P);
        for (auto& d : D)
            d = dist3(mt);
        for (auto& s : S)
            s = dist3(mt);
        int cheater = distP(mt);
        cheaters.push_back(cheater);
        for (int p = 0; p < P; p++) {
            if (p == cheater) {
                for (int q = 0; q < Q; q++) {
                    printf("%d", (coin(mt) || boold(eval(S[p] - D[q]))(mt)) ? 1 : 0);
                }
            } else {
                for (int q = 0; q < Q; q++) {
                    printf("%d", boold(eval(S[p] - D[q]))(mt) ? 1 : 0);
                }
            }
            printf("\n");
        }
    }
    ofstream answers("correct.txt");
    for (int i = 0; i < T; i++) {
        char s[100]{};
        sprintf(s, "Case #%d: %d\n", i + 1, cheaters[i] + 1);
        answers << s;
    }
    return 0;
}
