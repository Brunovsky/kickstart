#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 5000

double C[MAXN + 1][MAXN + 1];
double S[MAXN + 1][MAXN + 1]; // S[len][i] = SUM C[k][i] for k=0...len

void setup() {
    for (int len = 2; len <= MAXN; len++) {
        for (int i = 0; i < len; i++) {
            C[len][i] = S[len - 1][i] + S[len - 1][len - 1 - i];
            C[len][i] = 1 + C[len][i] / (len - 1);
            S[len][i] = S[len - 1][i] + C[len][i];
        }
    }
}

auto solve() {
    int N;
    cin >> N;
    vector<long> A(N);
    for (int i = 0; i < N; i++)
        cin >> A[i];

    double sum = 0;
    for (int i = 0; i < N; i++)
        sum += A[i] * C[N][i];

    return sum;
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    cout << setprecision(9) << fixed;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
