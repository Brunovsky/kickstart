#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    long C, D, V;
    cin >> C >> D >> V;
    vector<long> money(D);
    for (int i = 0; i < D; i++)
        cin >> money[i];

    sort(begin(money), end(money));

    int added = 0, m = 0;
    long R = 1; // smallest unrepresentable

    while (R <= V) {
        if (m < D && money[m] <= R) {
            R = R + money[m++] * C;
        } else {
            money.insert(begin(money) + m, R), D++;
            R = R + money[m++] * C;
            added++;
        }
    }

    return added;
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
