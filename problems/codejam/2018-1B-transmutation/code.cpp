#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int M;
    cin >> M;
    vector<array<int, 2>> recipe(M);
    vector<long> G(M);
    for (auto& [a, b] : recipe)
        cin >> a >> b, a--, b--;
    for (auto& g : G)
        cin >> g;

    long min_lead = G[0];
    long max_lead = accumulate(begin(G), end(G), 0L);
    long inf = max_lead;

    while (min_lead <= max_lead) {
        long lead = (min_lead + max_lead) / 2;
        vector<long> supply = G;
        vector<vector<long>> make(M, vector<long>(M, 0));

        supply[0] -= lead;
        for (int i = 0; i < M; i++) {
            make[i][recipe[i][0]] = make[i][recipe[i][1]] = 1;
        }

        while (true) {
            int i = 0;
            while (i < M && supply[i] >= 0)
                i++;
            if (i == M) {
                min_lead = lead + 1;
                break;
            } else if (make[i][i] > 0 || -supply[i] > inf) {
                max_lead = lead - 1;
                break;
            }
            for (int j = 0; j < M; j++) {
                supply[j] -= -supply[i] * make[i][j];
            }
            supply[i] = 0;
            for (int j = 0; j < M; j++) {
                if (make[j][i] > 0) {
                    for (int k = 0; k < M; k++) {
                        make[j][k] += make[j][i] * make[i][k];
                    }
                    make[j][i] = 0;
                }
            }
        }
    }

    return max_lead;
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
