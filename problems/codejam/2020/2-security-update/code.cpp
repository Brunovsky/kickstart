#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXC 101
#define MAXD 1001

int C, D;
int X[MAXC];
int edgelist[MAXD][2];
int weight[MAXC][MAXC];
int latency[MAXC];
vector<int> adj[MAXC];
map<int, int> cnt_ord;
multiset<int> dis_ord;
vector<int> dis_xmap[MAXD];
vector<int> cnt_xmap[MAXC];

void add_weights(int u) {
    for (int v : adj[u]) {
        if (v == 1 || latency[v]) {
            weight[u][v] = weight[v][u] = max(latency[u] - latency[v], 1);
        }
    }
    adj[u].clear();
}

auto solve() {
    cnt_ord.clear();
    dis_ord.clear();
    cin >> C >> D >> ws;
    for (int i = 2; i <= C; i++) {
        cin >> X[i];
        if (X[i] < 0) {
            cnt_ord[-X[i]]++;
            cnt_xmap[-X[i]].push_back(i);
        } else {
            dis_ord.insert(X[i]);
            dis_xmap[X[i]].push_back(i);
        }
    }
    for (int i = 0; i < D; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
        edgelist[i][0] = u;
        edgelist[i][1] = v;
    }
    int t = 1; // topological order
    int max_latency = 0;
    while (t < C) {
        int group = t;
        while (group < C && cnt_xmap[group].empty()) {
            group++;
        }
        while (t < group) {
            int dis = *dis_ord.begin();
            dis_ord.erase(dis_ord.begin());
            int u = dis_xmap[dis].back();
            dis_xmap[dis].pop_back();
            assert(dis == X[u] && dis >= max_latency);
            latency[u] = dis;
            max_latency = dis;
            add_weights(u);
            t++;
        }
        if (group < C) {
            max_latency++;
            for (uint i = 0; i < cnt_xmap[group].size(); i++) {
                int u = cnt_xmap[group][i];
                assert(group == -X[u]);
                latency[u] = max_latency;
                add_weights(u);
                t++;
            }
            cnt_xmap[group].clear();
        }
    }
    return 0;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        solve();
        cout << "Case #" << t << ":";
        for (int i = 0; i < D; i++) {
            int u = edgelist[i][0], v = edgelist[i][1];
            cout << ' ' << weight[u][v];
        }
        cout << '\n';
    }
    return 0;
}
