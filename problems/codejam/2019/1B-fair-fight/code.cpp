#include <bits/stdc++.h>

using namespace std;

// *****

#define ALL(s) begin(s), end(s)

struct RMQ {
    vector<vector<int>> jmp;
    RMQ(const vector<int>& V) : jmp(1, V) {
        for (uint pw = 1, k = 1; pw * 2 <= V.size(); pw *= 2, ++k) {
            jmp.emplace_back(V.size() - pw * 2 + 1);
            for (uint j = 0; j < jmp[k].size(); j++)
                jmp[k][j] = max(jmp[k - 1][j], jmp[k - 1][j + pw]);
        }
    }
    int query(int a, int b) {
        if (a == b)
            return INT_MAX;
        static constexpr int bits = CHAR_BIT * sizeof(int) - 1;
        int dep = bits - __builtin_clz(b - a);
        return max(jmp[dep][a], jmp[dep][b - (1 << dep)]);
    }
};

auto solve() {
    int N, K;
    cin >> N >> K;
    vector<int> C(N), D(N);
    for (int i = 0; i < N; i++)
        cin >> C[i];
    for (int i = 0; i < N; i++)
        cin >> D[i];
    vector<int> Cmap(N), cprev(N + 2), cnext(N + 2);

    RMQ rmq(D);

    iota(ALL(Cmap), 0);
    sort(ALL(Cmap), [&](int u, int v) { return C[u] < C[v]; });
    iota(ALL(cprev), -1);
    iota(ALL(cnext), 1);

    long ans = 0;

    for (int iteration = 0; iteration < N; iteration++) {
        int i = Cmap[iteration], j = i + 1;
        int L = cprev[j], R = cnext[j] - 2;
        cnext[cprev[j]] = cnext[j], cprev[cnext[j]] = cprev[j];

        if (D[i] > C[i] + K)
            continue;

        int lo, hi;

        lo = L, hi = i;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (rmq.query(mid, i + 1) > C[i] + K) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        int low_L = lo;
        // [low_L, i]: max <= C[i] + K

        lo = low_L, hi = i;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (rmq.query(mid, i + 1) < C[i] - K) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        }
        int high_L = lo;
        // [high_L, i]: max < C[i] - K

        lo = i, hi = R;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (rmq.query(i, mid + 1) > C[i] + K) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        }
        int high_R = hi;
        // [i, high_R]: max <= C[i] + K

        lo = i, hi = high_R;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (rmq.query(i, mid + 1) < C[i] - K) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        int low_R = hi;
        // [i, low_R]: max < C[i] - K

        ans += 1L * (high_L - low_L) * (high_R - i + 1);
        ans += 1L * (high_R - low_R) * (i - low_L + 1);
        ans -= 1L * (high_L - low_L) * (high_R - low_R);
    }

    return ans;
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
