#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

auto floyd_warshall(int V, const vector<tuple<int, int, int>>& edge) {
    constexpr int inf = numeric_limits<int>::max() / 2;

    vector<vector<int>> dist(V, vector<int>(V, inf));

    for (auto [u, v, w] : edge) {
        dist[u][v] = min(w, dist[u][v]);
    }
    for (int u = 0; u < V; u++) {
        dist[u][u] = 0;
    }

    unsigned long answer = 0;

    for (int k = 0; k < V; k++) {
        for (int u = 0; u < V; u++) {
            for (int v = 0; v < V; v++) {
                if (dist[u][v] > dist[u][k] + dist[k][v]) {
                    dist[u][v] = dist[u][k] + dist[k][v];
                    if (dist[u][v] > inf / 2) {
                        dist[u][v] = inf;
                    }
                }
                answer += dist[u][v] == inf ? 0 : dist[u][v];
            }
        }
    }

    return answer;
}

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int V, E;
    cin >> V >> E;
    vector<tuple<int, int, int>> edge(E);
    for (auto& [u, v, w] : edge) {
        cin >> u >> v >> w, u--, v--;
    }
    cout << floyd_warshall(V, edge) << '\n';
    return 0;
}
