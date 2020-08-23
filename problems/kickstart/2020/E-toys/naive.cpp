#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100000

struct Toy {
    long E, R;
};

int N;
Toy toys[MAXN];

auto solve() {
    cin >> N;
    assert(N <= MAXN);
    long sum = 0;
    for (int i = 0; i < N; i++) {
        cin >> toys[i].E >> toys[i].R;
        sum += toys[i].E;
    }
    long loop = 0;
    for (int i = 0; i < N; i++) {
        if (toys[i].R > sum - toys[i].E)
            break;
        loop += toys[i].E;
    }
    if (loop == sum) {
        return "0 INDEFINITELY"s;
    }
    int best_removed_inf = N, best_removed = 0;
    long best = sum;
    for (int mask = 1; mask < (1 << N); mask++) {
        vector<Toy> subset;
        int b = __builtin_popcount(mask);
        long subset_loop = 0;
        long subset_sum = 0;

        for (int i = 0; i < N; i++)
            if (mask & (1 << i))
                subset.push_back(toys[i]), subset_sum += toys[i].E;

        for (int i = 0; i < b; i++) {
            if (subset[i].R > subset_sum - subset[i].E) {
                break;
            }
            subset_loop += subset[i].E;
        }
        if (subset_loop == subset_sum) {
            best_removed_inf = min(best_removed_inf, N - b);
        } else if (best < subset_sum + subset_loop ||
                   (best == subset_sum + subset_loop && N - b < best_removed)) {
            best = subset_sum + subset_loop;
            best_removed = N - b;
        }
    }
    if (best_removed_inf < N) {
        return to_string(best_removed_inf) + " INDEFINITELY"s;
    } else {
        return to_string(best_removed) + " " + to_string(best);
    }
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
