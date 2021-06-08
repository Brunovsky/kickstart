#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

vector<vector<int>> tree;
vector<bool> mated;
int mates = 0;

void dfs(int u, int p) {
    for (int v : tree[u]) {
        if (v != p) {
            dfs(v, u);
        }
    }
    if (p != -1 && !mated[u] && !mated[p]) {
        mated[u] = mated[p] = true;
        mates++;
    }
}

int main() {
    ios::sync_with_stdio(false);
    int N;
    cin >> N;
    tree.assign(N, {});
    mated.assign(N, false);
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v, u--, v--;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }
    dfs(0, -1);
    cout << mates << endl;
    return 0;
}
