#include <bits/stdc++.h>

using namespace std;

// *****

double compute_probability(int N, int K, const vector<double>& p) {
    vector<double> q(N + 1, 0); // q[k]: probability of k successes
    q[0] = 1;
    for (int i = 0; i < N; i++) {
        for (int k = N; k > 0; k--) {
            q[k] = q[k] * (1 - p[i]) + q[k - 1] * p[i];
        }
        q[0] *= 1 - p[i];
    }
    double sum = 0;
    for (int k = K; k <= N; k++) {
        sum += q[k];
    }
    return sum;
}

auto solve() {
    int N, K;
    double U;
    cin >> N >> K >> U;

    vector<double> P(N);
    for (auto& x : P)
        cin >> x;
    sort(begin(P), end(P));
    if (U <= 0.0)
        return compute_probability(N, K, P);

    double best = 0;

    // leave [0..n) be ; increment the smallest among [n..N)
    for (int n = 0; n <= N; n++) {
        vector<double> p = P;
        p.push_back(1.0);
        double u = U;
        for (int j = n + 1; j <= N; j++) {
            int cnt_before = j - n;
            double p_gap = p[j] - p[n];
            if (u >= p_gap * cnt_before) {
                for (int i = n; i < j; i++) {
                    p[i] = p[j];
                }
                u -= p_gap * cnt_before;
            } else {
                for (int i = n; i < j; i++) {
                    p[i] += u / cnt_before;
                }
                u = 0;
                break;
            }
        }
        for (int j = n; j >= 0 && u > 0; j--) {
            if (u > 1.0 - p[j]) {
                u -= 1.0 - p[j], p[j] = 1.0;
            } else {
                p[j] += u, u = 0;
                break;
            }
        }
        best = max(best, compute_probability(N, K, p));
    }

    return best;
}

// *****

int main() {
    cout << setprecision(7) << showpoint << fixed;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
