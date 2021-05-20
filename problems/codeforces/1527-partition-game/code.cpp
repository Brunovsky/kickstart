#include <bits/stdc++.h>

using namespace std;

#define long int64_t

// *****

int maindp[35001][101];

auto solve() {
    int N;
    cin >> N;
    vector<int> A(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> A[i];
    }
    vector<int> first(N + 1), last(N + 1);

    for (int r = 1; r <= N; r++) {

        for (int k = 1; k <= 100; k++) {
            maindp[r][k] =
        }
    }

    return 0;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    cout << solve() << endl;
    return 0;
}
