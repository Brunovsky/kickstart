#include <bits/stdc++.h>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

// *****

using happy = pair<long, long>;

using max_tree = tree<happy, null_type, greater<happy>, rb_tree_tag,
                      tree_order_statistics_node_update>;

#define MAXN 20

int N;
long H;
long A[MAXN];
long B[MAXN];

auto populate(int M1, int M2) {
    vector<happy> sums{{0, 0}};
    for (int i = M1; i < M2; i++) {
        int S = sums.size();
        sums.resize(3 * S);
        for (int j = 0; j < S; j++) {
            sums[j + 1 * S] = {sums[j].first + A[i], sums[j].second};
            sums[j + 2 * S] = {sums[j].first, sums[j].second + B[i]};
            sums[j + 0 * S] = {sums[j].first + A[i], sums[j].second + B[i]};
        }
    }
    return sums;
}

auto solve() {
    cin >> N >> H >> ws;
    for (int i = 0; i < N; i++) {
        cin >> A[i] >> ws;
    }
    for (int i = 0; i < N; i++) {
        cin >> B[i] >> ws;
    }

    auto L1 = populate(0, N / 2);
    auto L2 = populate(N / 2, N);
    sort(begin(L1), end(L1), less<happy>{});
    sort(begin(L2), end(L2), greater<happy>{});
    int S1 = L1.size();
    int S2 = L2.size();

    size_t count = 0;
    long max_a = -1;
    int i = 0, j = 0;
    max_tree tree;

    while (i < S1) {
        long a1 = L1[i].first, b1 = L1[i].second;
        if (a1 > max_a) {
            while (j < S2 && L2[j].first + a1 >= H) {
                long b2 = L2[j].second;
                tree.insert({b2, j});
                j++;
            }
            max_a = a1;
        }
        count += tree.order_of_key({H - b1, -1});
        i++;
    }

    return count;
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
