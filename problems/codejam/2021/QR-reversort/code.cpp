#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100
int L[MAXN];

auto solve() {
    int N;
    cin >> N;
    for (int i = 0; i < N; i++)
        cin >> L[i];
    int cost = 0;
    for (int i = 0; i + 1 < N; i++) {
        int j = min_element(L + i, L + N) - L;
        cost += j - i + 1;
        reverse(L + i, L + j + 1);
    }
    return cost;
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
