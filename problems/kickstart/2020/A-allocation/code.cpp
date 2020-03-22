#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, B;
    cin >> N >> B;
    vector<int> A(N, 0);
    for (int i = 0; i < N; ++i)
        cin >> A[i];

    sort(A.begin(), A.end());
    int i = 0, sum = 0;
    while (i < N && sum + A[i] <= B)
        sum += A[i++];
    return i;
}

// *****

void reparse_test() {}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        reparse_test();
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
