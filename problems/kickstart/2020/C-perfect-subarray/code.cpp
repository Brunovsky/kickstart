#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'000
#define MAX 10'000'000

int N;
int A[MAXN];
int c[2 * MAX + 1];

auto solve() {
    memset(c, 0, sizeof(c));
    cin >> N >> ws;

    int positive = 0;
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
        positive += max(A[i], 0);
    }

    uint64_t total = 0;
    int sum = 0;
    c[MAX]++;
    for (int i = 0; i < N; i++) {
        sum += A[i];
        for (int n = 0; n * n <= positive; n++) {
            // sum - prefix = n ==> prefix = sum - n
            total += c[MAX + sum - n * n];
        }
        c[MAX + sum]++;
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
