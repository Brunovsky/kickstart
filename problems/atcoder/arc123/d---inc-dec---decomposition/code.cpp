#include <bits/stdc++.h>
#ifdef LOCAL
#include "code/formatting.hpp"
#else
#define debug(...) (void)0
#endif

using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

int main() {
    ios::sync_with_stdio(false), cin.tie(nullptr);
    int N;
    cin >> N;
    vector<int> A(N);
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    vector<long> B(N), C(N);
    B[0] = A[0];
    for (int i = 1; i < N; i++) {
        B[i] = B[i - 1];
        C[i] = C[i - 1];
        if (A[i - 1] < A[i]) {
            B[i] = B[i - 1] + (A[i] - A[i - 1]);
        } else if (A[i - 1] > A[i]) {
            C[i] = C[i - 1] + (A[i] - A[i - 1]);
        }

        assert(B[i] + C[i] == A[i]);
    }

    if (B[0] < 0 || C[N - 1] > 0) {
        long increment = max(-B[0], C[N - 1]);
        for (int i = 0; i < N; i++) {
            B[i] += increment;
            C[i] -= increment;
        }
    }
    for (int i = 0; i < N; i++) {
        assert(B[i] >= 0 && C[i] <= 0);
    }

    long ans = LONG_MAX;

    // subtract B[i] from all B, add B[i] to all C
    auto optimize = [&]() {
        vector<long> bpref(N + 1), cpref(N + 1);
        for (int i = 0; i < N; i++) {
            bpref[i + 1] = bpref[i] + B[i];
            cpref[i + 1] = cpref[i] + C[i];
        }

        for (int i = 0; i < N; i++) {
            long k = B[i];
            auto it = lower_bound(rbegin(C), rend(C), -B[i]);
            int j = N - (it - rbegin(C)); // 0 to N

            long right_B = bpref[N] - bpref[i] - k * (N - i); // positive
            long left_B = bpref[i] - k * i;                   // negative
            long left_C = cpref[j] + k * j;                   // positive
            long right_C = cpref[N] - cpref[j] + k * (N - j); // negative
            long sum = abs(right_B) + abs(left_B) + abs(left_C) + abs(right_C);
            if (ans > sum) {
                ans = sum;
            }
        }
    };
    optimize();

    // now make B negative, C positive, and reverse them
    long decrement = max(B[0], -C[N - 1]);
    for (int i = 0; i < N; i++) {
        B[i] -= decrement;
        C[i] += decrement;
    }
    reverse(begin(B), end(B));
    reverse(begin(C), end(C));
    swap(B, C);
    optimize();

    cout << ans << '\n';
    return 0;
}
