#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 100'001

int N, K;
int diff[MAXN];
int split[MAXN];
int M[MAXN];

int quot(int i) {
    return (diff[i] + split[i]) / (split[i] + 1);
}

bool cmp(int l, int r) {
    int dquot = quot(l) - quot(r);
    if (dquot != 0) {
        return dquot > 0;
    }

    int ddiff = diff[l] - diff[r];
    if (ddiff != 0) {
        return ddiff < 0;
    }

    return l < r;
}

auto solve() {
    memset(diff, 0, sizeof(diff));
    memset(split, 0, sizeof(split));
    memset(M, 0, sizeof(M));

    cin >> N >> K;

    for (int i = 0; i < N; ++i) {
        cin >> M[i];
    }

    using cmp_t = function<bool(int, int)>;
    set<int, cmp_t> indices(cmp);
    for (int i = 0; i < N - 1; ++i) {
        diff[i] = M[i + 1] - M[i];
        indices.insert(i);
    }

    for (int k = 0; k < K; ++k) {
        const int i = *indices.begin();
        indices.erase(indices.begin());
        ++split[i];
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
