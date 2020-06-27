#include <bits/stdc++.h>

using namespace std;

// *****

// Kruskal with weak disjoint-set

struct edge_t {
    uint i, j, w;
};

auto solve() {
    uint N;
    cin >> N;
    vector<uint> A(N), B(N);
    for (uint i = 0; i < N; ++i)
        cin >> A[i];
    for (uint i = 0; i < N; ++i)
        cin >> B[i];

    vector<edge_t> adj(N * (N - 1) / 2);
    uint k = 0;
    for (uint i = 0; i < N; ++i)
        for (uint j = i + 1; j < N; ++j)
            adj[k++] = {i, j, min(A[i] ^ B[j], A[j] ^ B[i])};
    assert(k == N * (N - 1) / 2);

    sort(adj.begin(), adj.end(), [&](edge_t u, edge_t v) {
        return tie(u.w, u.i, u.j) < tie(v.w, v.i, v.j);
    });

    vector<uint> next(N);
    iota(next.begin(), next.end(), 0);

    auto find = [&](uint i) {
        while (i != next[i])
            i = next[i] = next[next[i]];
        return i;
    };

    auto join = [&](uint i, uint j) { return next[find(j)] = find(i); };

    uint64_t sum = 0, n = 0, edges = 0;

    while (edges < N - 1) {
        uint i = adj[n].i, j = adj[n].j, w = adj[n].w;
        ++n;
        if (find(i) == find(j))
            continue;
        sum += w;
        join(i, j);
        ++edges;
    }
    return sum;
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
