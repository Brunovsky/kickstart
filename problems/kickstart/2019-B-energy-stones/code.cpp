#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100

struct Stone {
    int S, E, I;
};

int N;
Stone stones[MAXN];

bool cmp_stone(const Stone &Ci, const Stone &Cj) {
    return Ci.I * Cj.S > Cj.I * Ci.S;
}

auto solve() {
    cin >> N >> ws;
    for (int n = 0; n < N; ++n) {
        cin >> stones[n].S >> stones[n].E >> stones[n].I >> ws;
    }

    int maxT = 0;
    for (int i = 0; i < N; i++) {
        maxT += stones[i].S;
    }
    sort(stones, stones + N, cmp_stone);

    vector<int> tail_energy(maxT + 1, 0);
    vector<int> head_energy(maxT + 1, 0);

    // using only stones[k..], at time t what is the maximum energy we can get?
    for (int k = N - 1; k >= 0; --k) {
        int t = 0;
        Stone stone = stones[k];
        while (t <= maxT - stone.S) {
            int decay = stone.I * t;
            if (decay >= stone.E) {
                break;
            }
            int energy = stone.E - decay;
            head_energy[t] = max(tail_energy[t + stone.S] + energy, tail_energy[t]);
            ++t;
        }
        while (t < maxT) {
            head_energy[t] = tail_energy[t];
            ++t;
        }
        swap(head_energy, tail_energy);
    }

    return tail_energy[0];
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
