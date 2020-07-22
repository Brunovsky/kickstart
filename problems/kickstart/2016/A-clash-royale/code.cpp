#include <bits/stdc++.h>

using namespace std;

// *****

using i64 = int64_t;

#define MAX 13

i64 M;
int N;
int K[MAX];
int L[MAX];
i64 A[MAX][MAX];
i64 C[MAX][MAX];

using upgrade_t = pair<i64, i64>;
#define COST first
#define ATTACK second

auto combine(int N1, int N2) {
    // upgrades[c]: upgrade combinations with c cards from range [N1,N2)
    vector<vector<upgrade_t>> upgrades;
    upgrades.resize(N2 - N1 + 1, {});
    upgrades[0].push_back({0, 0});

    for (int i = N1; i < N2; i++) {
        // careful, go decreasing in #cards here
        for (int c = i - N1; c >= 0; c--) {
            int S = upgrades[c].size();
            for (int j = 0; j < S; j++) {
                i64 cost = upgrades[c][j].COST;
                i64 attack = upgrades[c][j].ATTACK + A[i][L[i]];
                upgrades[c + 1].push_back({cost, attack});

                for (int l = L[i]; l < K[i]; l++) {
                    // upgrade from level l to level l + 1
                    cost += C[i][l];
                    attack += A[i][l + 1] - A[i][l];
                    upgrades[c + 1].push_back({cost, attack});
                }
            }
        }
    }

    for (int c = 0; c <= N2 - N1; c++) {
        auto &up = upgrades[c];
        sort(up.begin(), up.end());

        int S = up.size();
        int read = 0, write = 0;
        i64 max_attack = -1;

        // remove combinations whose attack is not the largest for its cost
        while (read < S) {
            if (max_attack < up[read].ATTACK) {
                max_attack = up[read].ATTACK;
                up[write++] = up[read];
            }
            read++;
        }

        up.resize(write);
        up.shrink_to_fit();
    }

    return upgrades;
}

auto solve() {
    cin >> M >> N >> ws;
    for (int i = 0; i < N; i++) {
        cin >> K[i] >> L[i] >> ws;
        for (int j = 1; j <= K[i]; j++) {
            cin >> A[i][j];
        }
        for (int j = 1; j < K[i]; j++) {
            cin >> C[i][j];
        }
    }

    auto L1s = combine(0, N / 2);
    auto L2s = combine(N / 2, N);

    i64 best = 0;
    for (size_t cl = 0; cl <= 8; cl++) {
        size_t cr = 8 - cl;
        if (cl >= L1s.size() || cr >= L2s.size()) {
            continue;
        }
        auto &L1 = L1s[cl];
        auto &L2 = L2s[cr];
        reverse(L2.begin(), L2.end());
        int S1 = L1.size();
        int S2 = L2.size();
        int i = 0, j = 0;

        while (i < S1 && j < S2) {
            while (j < S2 && L1[i].COST + L2[j].COST > M) {
                j++;
            }
            if (j == S2) {
                break;
            }
            while (i < S1 && L1[i].COST + L2[j].COST <= M) {
                i++;
            }
            i64 attack = L1[i - 1].ATTACK + L2[j].ATTACK;
            best = max(best, attack);
        }
    }
    return best;
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
