#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> indeg(N);
    for (int i = 0; i < N; i++)
        cin >> indeg[i];
    vector<int> node(N);
    iota(begin(node), end(node), 0);
    sort(begin(node), end(node), [&](int u, int v) { return indeg[u] < indeg[v]; });
    int l = 0, r = N - 1;
    while (indeg[l] == 0)
        l++;
    while (indeg[r] == N - 1)
        r--;

    string ans;
    char stub[100]{};

    while (l < r) {
        int L = l + 1, R = r - 1;
        while (L < N && indeg[l] == indeg[L])
            L++;
        while (R >= 0 && indeg[r] == indeg[R])
            R--;
        for (int i = l; i < L; i++) {
            for (int j = r; j > R; j--) {
                if (i < j) {
                    sprintf(stub, "? %d %d", j + 1, i + 1);
                    cout << stub << endl;
                    cin >> ans;
                    if (ans == "YES") {
                        sprintf(stub, "! %d %d", j + 1, i + 1);
                        cout << stub << endl;
                        return;
                    }
                }
            }
        }
        l = L, r = R;
    }

    sprintf(stub, "! %d %d", 0, 0);
    cout << stub << endl;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    solve();
    return 0;
}
