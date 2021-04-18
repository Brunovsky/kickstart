#include <bits/stdc++.h>

using namespace std;

// *****

template <typename T>
struct RMQ {
    vector<vector<T>> jmp;
    RMQ(const vector<T>& V) : jmp(1, V) {
        for (uint pw = 1, k = 1; pw * 2 <= V.size(); pw *= 2, ++k) {
            jmp.emplace_back(V.size() - pw * 2 + 1);
            for (uint j = 0; j < jmp[k].size(); j++)
                jmp[k][j] = min(jmp[k - 1][j], jmp[k - 1][j + pw]);
        }
    }
    T query(int a, int b) {
        assert(a < b); // or return inf if a == b
        static constexpr int bits = CHAR_BIT * sizeof(int) - 1;
        int dep = bits - __builtin_clz(b - a);
        return min(jmp[dep][a], jmp[dep][b - (1 << dep)]);
    }
};

auto solve() {
    int N, p;
    cin >> N >> p;
    vector<int> A(N);
    for (int i = 0; i < N; i++)
        cin >> A[i];

    RMQ<int> rmq(A);
    vector<bool> vis(V, false);

    vector<int> index(N);
    iota(begin(index), end(index), 0);
    sort(begin(index), end(index), [&](int u, int v) { return A[u] < A[v]; });

    for (int i = 0; i < N - 1; i++) {
        int u = index[i];
    }

    long msf = 0;
    for (int n = 0; n < V; n++) {
        if (vis[n])
            continue;

        priority_queue<pair<int, int>> Q;
        Q.push({0, n});

        while (!Q.empty()) {
            auto [neg_weight, u] = Q.top();
            Q.pop();
            if (vis[u])
                continue;

            vis[u] = true;
            msf += -neg_weight;
            for (auto [v, w] : adj[u])
                if (!vis[v])
                    Q.push({-w, v});
        }
    }
    return msf;

    return 0;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
    }
    return 0;
}
