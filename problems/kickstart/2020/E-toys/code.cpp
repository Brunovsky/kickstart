#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100000

struct Toy {
    long E, R;

    // need to break ties
    friend bool operator<(Toy a, Toy b) {
        return make_pair(a.sum(), -a.E) > make_pair(b.sum(), -b.E);
    }
    long sum() const { return E + R; }
};

int N;
Toy toys[MAXN];

// Naive says:
// To get INDEFINITELY, select k items such that
//      E[i] + R[i] <= sum(E)     where k <= N is maximal.
// if this holds for k = N, great, it is INDEFINITELY. Otherwise remove in any order the
// items that break this condition, updating the condition, until we have none left.
// If we finish with items remaining that's the solution and iti s INDEFINITELY.
// If we end up removing all items, we fail and it is not INDEFINITELY, so we just have
// to figure out which of the items to remove.

auto solve() {
    cin >> N;
    assert(N <= MAXN);
    long sum = 0;
    for (int i = 0; i < N; i++) {
        cin >> toys[i].E >> toys[i].R;
        sum += toys[i].E;
    }
    // best in case it is not forever
    multiset<Toy> accepted;
    long best = sum, removed_best = 0;
    long remaining = sum, prefix = 0, removed = 0;
    for (int i = 0; i < N; i++) {
        if (toys[i].sum() <= remaining) {
            prefix += toys[i].E;
            accepted.insert(toys[i]);
        } else {
            remaining -= toys[i].E;
            removed++;
            while (!accepted.empty() && accepted.begin()->sum() > remaining) {
                remaining -= accepted.begin()->E;
                prefix -= accepted.begin()->E;
                accepted.erase(accepted.begin());
                removed++;
            }
        }
        if (best < remaining + prefix) {
            best = remaining + prefix;
            removed_best = removed;
        }
    }
    assert(removed + accepted.size() == uint(N));
    // now check if it is actually forever
    sort(toys, toys + N);
    remaining = sum;
    for (int i = 0; i < N; i++) {
        if (toys[i].sum() <= remaining)
            return to_string(i) + " INDEFINITELY"s;
        remaining -= toys[i].E;
    }
    // return finite best
    return to_string(removed_best) + " " + to_string(best);
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
