#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

using vi = vector<int>;
using vvi = vector<vector<int>>;
constexpr int MAXN = 50000;
bitset<MAXN> reachable[MAXN];
stack<int> S;
int timer = 1;

void dfs(int u, const vvi& out, vi& index, vi& lowlink, vvi& cset, vi& cmap) {
    index[u] = lowlink[u] = timer++;
    S.push(u);

    for (int v : out[u]) {
        if (index[v] && cmap[v] == -1) { // <-- skip cmap'd nodes
            lowlink[u] = min(lowlink[u], index[v]);
        } else if (!index[v]) {
            dfs(v, out, index, lowlink, cset, cmap);
            lowlink[u] = min(lowlink[u], lowlink[v]);
        }
    }

    // found a strongly connected component
    if (index[u] == lowlink[u]) {
        int c = cset.size();
        cset.push_back({});
        int v;
        do {
            v = S.top(), S.pop();
            cmap[v] = c, cset[c].push_back(v);
        } while (u != v);
    }
}

int main() {
    ios::sync_with_stdio(false);
    int N, M, Q;
    cin >> N >> M >> Q;
    vvi out(N);
    vvi cset;
    vi cmap(N, -1);
    vi index(N);
    vi lowlink(N);
    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        out[u].push_back(v);
    }
    for (int u = 0; u < N; u++) {
        if (!index[u]) {
            dfs(u, out, index, lowlink, cset, cmap);
        }
    }
    int C = cset.size();
    set<array<int, 2>> edges;
    for (int u = 0; u < N; u++) {
        for (int v : out[u]) {
            if (cmap[u] != cmap[v]) {
                edges.insert({cmap[u], cmap[v]});
            }
        }
    }
    vector<vector<int>> sin(C);
    vector<int> soutdeg(C);
    vector<int> rbfs(C);
    for (auto [u, v] : edges) {
        soutdeg[u]++;
        sin[v].push_back(u);
    }
    int s = 0, T = 0;
    for (int u = 0; u < C; u++) {
        if (soutdeg[u] == 0) {
            rbfs[T++] = u;
        }
        reachable[u].set(u);
    }
    while (s < T) {
        int u = rbfs[s++];
        for (int v : sin[u]) {
            reachable[v] |= reachable[u];
            if (--soutdeg[v] == 0) {
                rbfs[T++] = v;
            }
        }
    }
    while (Q--) {
        int a, b;
        cin >> a >> b, a--, b--;
        a = cmap[a], b = cmap[b];
        if (reachable[a][b]) {
            cout << "YES" << endl;
        } else {
            cout << "NO" << endl;
        }
    }

    return 0;
}
