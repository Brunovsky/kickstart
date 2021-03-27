#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 105

int max_cost[MAXN];
int id[MAXN];
int output[MAXN];

void setup() {
    for (int N = 2; N <= 100; N++) {
        max_cost[N] = N + max_cost[N - 1];
    }
}

auto solve() {
    int N, C;
    cin >> N >> C;
    iota(id, id + N, 0);
    if (C < N - 1 || C > max_cost[N]) {
        cout << " IMPOSSIBLE\n";
        return;
    }

    for (int i = 0; i + 1 < N; i++) {
        int M = N - i;
        int c = max(1, C - max_cost[M - 1]);
        assert(c <= M);
        output[id[i + c - 1]] = i + 1;
        reverse(id + i, id + i + c);
        C -= c;
    }
    output[id[N - 1]] = N;
    assert(C == 0);

    for (int i = 0; i < N; i++)
        cout << ' ' << output[i];
    cout << "\n";
}

// *****

int main() {
    setup();
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":";
        solve();
    }
    return 0;
}
