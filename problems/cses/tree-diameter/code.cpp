#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int find_tree_diameter(const vector<vector<int>>& tree) {
    int V = tree.size();
    vector<int> degree(V), dist(V, 0), bfs(V), prev(V, -1);
    int i = 0, S = 0;

    for (int u = 0; u < V; u++) {
        degree[u] = tree[u].size();
        if (degree[u] == 1) {
            bfs[S++] = u;
        }
    }

    while (i < S) {
        int u = bfs[i++];
        for (int v : tree[u]) {
            if (--degree[v] == 1) {
                bfs[S++] = v;
                dist[v] = dist[u] + 1;
                prev[v] = u;
            }
        }
    }
    // up to 2 centers.
    assert((V == 1 || S == V) && (V < 3 || dist[bfs[V - 3]] != dist[bfs[V - 1]]));

    vector<int> diameter;

    if (V > 0) {
        int u = bfs[V - 1];
        do {
            diameter.push_back(u);
            u = prev[u];
        } while (u != -1);
    }
    if (V > 1) {
        int u = bfs[V - 2];
        reverse(begin(diameter), end(diameter));
        do {
            diameter.push_back(u);
            u = prev[u];
        } while (u != -1);
    }

    return diameter.size() - 1;
}

int main() {
    ios::sync_with_stdio(false);
    int N;
    cin >> N;
    vector<vector<int>> tree(N);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }
    cout << find_tree_diameter(tree) << endl;
    return 0;
}
