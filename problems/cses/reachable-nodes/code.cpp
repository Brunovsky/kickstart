#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

constexpr int MAXN = 50000;
bitset<MAXN> reachable[MAXN];

int main() {
    ios::sync_with_stdio(false);
    int N, M;
    cin >> N >> M;
    vector<vector<int>> in(N);
    vector<int> out(N);
    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        in[v].push_back(u);
        out[u]++;
    }
    vector<int> rbfs(N);
    int s = 0, S = 0;
    for (int u = 0; u < N; u++) {
        if (out[u] == 0) {
            rbfs[S++] = u;
        }
        reachable[u].set(u);
    }
    while (s < S) {
        int u = rbfs[s++];
        for (int v : in[u]) {
            reachable[v] |= reachable[u];
            if (--out[v] == 0) {
                rbfs[S++] = v;
            }
        }
    }
    for (int u = 0; u < N; u++) {
        cout << reachable[u].count() << " \n"[u + 1 == N];
    }

    return 0;
}
