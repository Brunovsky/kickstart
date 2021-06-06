#include <bits/stdc++.h>

using namespace std;

// *****

// Solution according to analysis

#define MAXV 51

int V, E;
int adj[MAXV];
int len[MAXV];
int A0, B0;

auto solve() {
    cin >> V >> E >> ws;
    memset(adj, 0x7f, sizeof(adj));
    memset(len, 0x7f, sizeof(len));
    A0 = B0 = 0;

    for (int i = 0; i < E; ++i) {
        int a, b, l;
        cin >> a >> b >> l;
        if (l < len[a]) {
            adj[a] = b;
            len[a] = l;
        }
        if (l < len[b]) {
            adj[b] = a;
            len[b] = l;
        }
        if (l == 0) {
            A0 = a;
            B0 = b;
        }
    }

    ulong sets = 0;

    for (int i = 1; i <= V; ++i) {
        if (i == adj[adj[i]]) {
            ++sets;
        }
    }

    sets /= 2;

    if (A0 > 0) {
        for (int i = 1; i <= V; ++i) {
            if (adj[i] == A0 || adj[i] == B0) {
                ++sets;
            }
        }
        sets -= 2;
    }

    return ulong(1) << sets;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
