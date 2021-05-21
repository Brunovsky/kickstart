#include <bits/stdc++.h>

using namespace std;

// *****

int M;
long C;
vector<int> P;

double compute(double b) {
    double sum = 0.0;
    double pow = 1.0;
    for (int i = 0; i < M; ++i) {
        sum += P[i] * pow;
        pow *= b;
    }
    sum -= C * pow;
    return sum;
}

auto solve() {
    cin >> M >> C;
    P.resize(M);
    for (int i = M - 1; i >= 0; --i) {
        cin >> P[i];
    }

    int iterations = 70;
    double l = 0.0, r = 2.0;

    do {
        double b = (l + r) / 2.0;
        if (compute(b) > 0.0) {
            l = b;
        } else {
            r = b;
        }
    } while (--iterations && abs(r - l) > 1e-11);

    return l - 1.0;
}

// *****

int main() {
    unsigned T;
    cout << fixed << showpoint << setprecision(9);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
