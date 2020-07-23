#include <bits/stdc++.h>

using namespace std;

// *****

struct edge_t {
    int i, j, w;
};

bool operator<(edge_t u, edge_t v) {
    return make_tuple(u.w, u.i, u.j) < make_tuple(v.w, v.i, v.j);
}

vector<int> parent;

int find(int i) {
    while (i != parent[i]) {
        i = parent[i] = parent[parent[i]];
    }
    return i;
}

int join(int i, int j) {
    return parent[find(j)] = find(i);
}

auto solve() {
    int N;
    cin >> N;
    vector<int> A(N), B(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }
    for (int i = 0; i < N; ++i) {
        cin >> B[i];
    }

    vector<edge_t> adj(N * (N - 1) / 2);
    int k = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            adj[k++] = {i, j, min(A[i] ^ B[j], A[j] ^ B[i])};
        }
    }
    sort(begin(adj), end(adj));

    parent.assign(N, {});
    iota(begin(parent), end(parent), 0);

    long sum = 0, n = 0, edges = 0;

    while (edges < N - 1) {
        int i = adj[n].i, j = adj[n].j, w = adj[n].w;
        ++n;
        if (find(i) == find(j)) {
            continue;
        }
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
