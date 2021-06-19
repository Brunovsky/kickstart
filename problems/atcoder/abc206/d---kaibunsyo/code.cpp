#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }
    unordered_map<int, unordered_set<int>> adj;
    unordered_set<int> nums;
    for (int i = 0, j = N - 1; i < j; i++, j--) {
        if (a[i] != a[j]) {
            adj[a[i]].insert(a[j]);
            adj[a[j]].insert(a[i]);
            nums.insert(a[i]);
            nums.insert(a[j]);
        }
    }
    unordered_map<int, bool> vis;
    int components = 0;
    vector<int> bfs(nums.size());
    for (int u : nums) {
        if (!vis[u]) {
            int i = 0, S = 1;
            vis[u] = true;
            bfs[0] = u;
            while (i < S) {
                for (int v : adj[bfs[i++]]) {
                    if (!vis[v]) {
                        vis[v] = true;
                        bfs[S++] = v;
                    }
                }
            }
            components++;
        }
    }
    cout << nums.size() - components << '\n';
    return 0;
}
