#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000
#define MAX  10'000'000

int N;
int A[MAXN];
int cnt[2 * MAX + 1];

auto solve() {
    memset(cnt, 0, sizeof(cnt));
    cin >> N >> ws;

    int positive = 0;
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
        positive += max(A[i], 0);
    }

    ulong total = 0;
    int sum = 0;
    cnt[MAX]++;
    for (int i = 0; i < N; i++) {
        sum += A[i];
        for (int n = 0; n * n <= positive; n++) {
            // sum - prefix = n ==> prefix = sum - n
            total += cnt[MAX + sum - n * n];
        }
        cnt[MAX + sum]++;
    }
    return total;
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
