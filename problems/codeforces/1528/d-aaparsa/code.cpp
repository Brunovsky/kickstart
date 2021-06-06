#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

void setmin(int& a, int b) { a = min(a, b); }

auto solve() {
    int N, M;
    cin >> N >> M;
    vector<int> a(M), b(M), c(M);
    vector<vector<int>> cannons(N);
    for (int i = 0; i < M; i++) {
        cin >> a[i] >> b[i] >> c[i];
        cannons[a[i]].push_back(i);
    }

    vector<vector<int>> dist(N, vector<int>(N, INT_MAX / 2));

    for (int i = 0; i < M; i++) {
        for (int u = 0; u < N; u++) {
            setmin(dist[a[i]][(b[i] + u) % N], u + c[i]);
        }
    }
    for (int u = 0; u < N; u++) {
        dist[u][u] = 0;
    }

    // Floyd-Warshall
    for (int k = 0; k < N; k++) {
        for (int u = 0; u < N; u++) {
            for (int v = 0; v < N; v++) {
                if (dist[u][v] > dist[u][k] + dist[k][v]) {
                    dist[u][v] = dist[u][k] + dist[k][v];
                }
            }
        }
    }

    for (int u = 0; u < N; u++) {
        for (int v = 0; v < N; v++) {
            cout << dist[u][v] << " \n"[v + 1 == N];
        }
    }
}

// *****

int main() {
    ios::sync_with_stdio(false);
    solve();
    return 0;
}
