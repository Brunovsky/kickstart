#include <bits/stdc++.h>

using namespace std;

// *****

auto solve() {
    int N, K;
    vector<int> M;
    cin >> N >> K;
    M.assign(N, 0);
    for (int i = 0; i < N; ++i)
        cin >> M[i];

    const int D = N - 1;
    vector<int> dif(D, 0);
    for (int i = 0; i < D; ++i)
        dif[i] = M[i + 1] - M[i];

    vector<int> spl(D, 0);

    using cmp_t = function<bool(int, int)>;
    const auto quot = [&](int i) { return (dif[i] + spl[i]) / (spl[i] + 1); };
    const cmp_t cmp = [&](int l, int r) {
        const int quotdiff = quot(l) - quot(r);
        if (quotdiff != 0)
            return quotdiff > 0;

        const int difdiff = dif[l] - dif[r];
        if (difdiff != 0)
            return difdiff < 0;

        return l < r;
    };

    set<int, cmp_t> indices(cmp);
    for (int i = 0; i < D; ++i)
        indices.insert(i);

    for (int k = 0; k < K; ++k) {
        const int i = *indices.begin();
        indices.erase(indices.begin());
        ++spl[i];
        indices.insert(i);
    }

    return quot(*indices.begin());
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
