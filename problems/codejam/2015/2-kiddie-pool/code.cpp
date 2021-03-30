#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100
double R[MAXN], C[MAXN];

#define P(i) (R[i] * C[i])

auto solve1(double V, double X) {
    double rate, heat;
    cin >> rate >> heat;
    if (abs(X - heat) > 1e-7) {
        cout << "IMPOSSIBLE"s;
    } else {
        cout << V / rate;
    }
}

auto solve() {
    int N;
    double V, X;
    cin >> N >> V >> X;

    if (N == 1) {
        return solve1(V, X);
    }

    double R_sum = 0, P_sum = 0;
    for (int i = 0; i < N; i++) {
        cin >> R[i] >> C[i], R_sum += R[i], P_sum += P(i);
    }

    if (X < *min_element(C, C + N) || *max_element(C, C + N) < X) {
        cout << "IMPOSSIBLE"s;
        return;
    }

    double t = V / R_sum;
    double heat = t * P_sum / V;

    vector<int> src(N);
    iota(begin(src), end(src), 0);
    sort(begin(src), end(src), [X, heat](int i, int j) {
        return heat < X ? tie(C[i], i) < tie(C[j], j) : tie(C[i], i) > tie(C[j], j);
    });

    int i = 0;
    while (abs(heat - X) > 1e-10 && i + 1 < N) {
        int n = src[i++];
        R_sum = 0, P_sum = 0;
        for (int j = i; j < N; j++) {
            R_sum += R[src[j]], P_sum += P(src[j]);
        }
        double delta = R[n] * (P_sum - C[n] * R_sum) / (R_sum * V);
        double s = clamp((X - heat) / delta, 0.0, t);
        double t_inc = s * R[n] / R_sum;
        t = t + t_inc;
        heat += s * delta;
    }
    cout << t;
}

// *****

int main() {
    setbuf(stdout, nullptr);
    cout << setprecision(7) << fixed << showpoint;
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
        cout << endl;
    }
    return 0;
}
