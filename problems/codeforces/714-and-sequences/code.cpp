#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long MOD = 1e9 + 7;

auto solve() {
    int N;
    cin >> N;
    vector<unsigned> A(N);
    for (int i = 0; i < N; i++)
        cin >> A[i];

    unsigned sum = accumulate(begin(A), end(A), UINT_MAX, std::bit_and<unsigned>{});
    long n = count(begin(A), end(A), sum);
    long M = n * (n - 1) % MOD;
    for (int i = 1; i <= N - 2; i++)
        M = M * i % MOD;
    return M;
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << solve() << endl;
    }
    return 0;
}
