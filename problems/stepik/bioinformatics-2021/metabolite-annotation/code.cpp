#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    constexpr double inf = 1e9;
    int T;
    cin >> T;
    while (T--) {
        int M, K, N;
        cin >> M >> K >> N;
        vector<double> mass(M), adducts(K), signals(N);
        for (int i = 0; i < M; i++) {
            cin >> mass[i];
        }
        for (int i = 0; i < K; i++) {
            cin >> adducts[i];
        }
        for (int i = 0; i < N; i++) {
            cin >> signals[i];
        }
        if (1LL * M * K <= 100'000'000) {
            vector<tuple<double, int, int>> sums;
            sums.push_back({-inf, -1, -1});
            sums.push_back({inf, -1, -1});
            for (int m = 0; m < M; m++) {
                for (int a = 0; a < K; a++) {
                    if (mass[m] + adducts[a] > 0) {
                        sums.push_back({mass[m] + adducts[a], m + 1, a + 1});
                    }
                }
            }
            sort(begin(sums), end(sums));
            for (int s = 0; s < N; s++) {
                auto it = lower_bound(begin(sums), end(sums),
                                      make_tuple(signals[s], 0, 0));
                auto [above, m1, a1] = *it;
                auto [below, m2, a2] = *--it;
                if (abs(above - signals[s]) <= abs(signals[s] - below)) {
                    cout << m1 << ' ' << a1 << '\n';
                } else {
                    cout << m2 << ' ' << a2 << '\n';
                }
            }
        } else if (1LL * K * N <= 100'000'000) {
            vector<pair<double, int>> masses(M);
            for (int m = 0; m < M; m++) {
                masses[m] = {mass[m], m + 1};
            }
            masses.push_back({inf, -1});
            masses.push_back({-inf, -1});
            sort(begin(masses), end(masses));
            vector<pair<int, int>> best(N, make_pair(1, 1));
            vector<double> best_diff(N, inf);
            for (int s = 0; s < N; s++) {
                for (int a = 0; a < K; a++) {
                    // MIN(abs(s - (m + a))) = MIN(abs((s - a) - m))
                    double diff = signals[s] - adducts[a];
                    auto it = lower_bound(begin(masses), end(masses), make_pair(diff, 0));
                    auto [above, m1] = *it;
                    auto [below, m2] = *--it;
                    double above_diff = abs(above - diff);
                    double below_diff = abs(diff - below);
                    if (m1 + adducts[a] > 0) {
                        if (above_diff < best_diff[s]) {
                            best[s] = {m1, a + 1};
                            best_diff[s] = above_diff;
                        }
                    }
                    if (m2 + adducts[a] > 0) {
                        if (below_diff < best_diff[s]) {
                            best[s] = {m2, a + 1};
                            best_diff[s] = below_diff;
                        }
                    }
                }
            }
            for (int s = 0; s < N; s++) {
                auto [m, a] = best[s];
                cout << m << ' ' << a << '\n';
            }
        } else {
            assert(false);
        }
    }
    return 0;
}
