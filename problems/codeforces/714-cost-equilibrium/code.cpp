#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N;
    cin >> N;
    vector<int> A(N);
    for (int i = 0; i < N; i++)
        cin >> A[i];
    int64_t sum = accumulate(begin(A), end(A), 0LL);
    int each = sum / N;
    if (1L * each * N != sum)
        return 0LL;

    int64_t delta = 0;
    for (int i = 0; i < N; i++)
        delta += abs(A[i] - each);

    return 0LL;
}

// *****

int main() {
    ios::sync_with_stdio(false);
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << solve() << endl;
    }
    return 0;
}
