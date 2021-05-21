#include <bits/stdc++.h>

using namespace std;

// *****

struct floyd_warshall {
    int V, E;
    vector<int> source;
    vector<int> target;
    vector<int> weight;

    floyd_warshall(int V) : V(V), E(0) {}

    void add(int u, int v, int w) {
        assert(0 <= u && u < V && 0 <= v && v < V && u != v && w >= 0);
        source.push_back(u);
        target.push_back(v);
        weight.push_back(w);
        E++;
    }

    vector<vector<int>> dist;

    void compute() {
        dist.assign(V, vector<int>(V, INT_MAX / 2));

        for (int e = 0; e < E; e++) {
            int u = source[e], v = target[e], w = weight[e];
            dist[u][v] = w;
        }
        for (int u = 0; u < V; u++) {
            dist[u][u] = 0;
        }

        for (int k = 0; k < V; k++) {
            for (int u = 0; u < V; u++) {
                for (int v = 0; v < V; v++) {
                    if (dist[u][v] > dist[u][k] + dist[k][v]) {
                        dist[u][v] = dist[u][k] + dist[k][v];
                    }
                }
            }
        }
    }
};

auto solve() {
    int N, M;
    cin >> N >> M;
    floyd_warshall g(N);
    for (int i = 0; i < M; i++) {
        int u, v, w;
        cin >> u >> v >> w, u--, v--;
        g.add(u, v, w);
        g.add(v, u, w);
    }
    g.compute();
    for (int e = 0; e < g.E; e++) {
        int u = g.source[e], v = g.target[e], w = g.weight[e];
        if (g.dist[u][v] != w) {
            cout << "Impossible"s << endl;
            return;
        }
    }
    cout << M << endl;
    for (int e = 0; e < M; e++) {
        int u = g.source[2 * e], v = g.target[2 * e], w = g.weight[2 * e];
        cout << u + 1 << ' ' << v + 1 << ' ' << w << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ": ";
        solve();
    }
    return 0;
}
