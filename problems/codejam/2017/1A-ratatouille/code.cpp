#include <bits/stdc++.h>

using namespace std;

// *****

#define MAXN 50
#define MAXP 50

struct pkg {
    int n, p;
};

int N, P;
vector<long> amount[MAXN];
long recipe[MAXN];
int cur[MAXN];
pkg packages[MAXN * MAXP];

// ingred[a] / recipe[a] < ingred[b] / recipe[b]
bool operator<(pkg a, pkg b) {
    return amount[a.n][a.p] * recipe[b.n] < amount[b.n][b.p] * recipe[a.n];
}

auto solve() {
    memset(cur, 0, sizeof(cur));
    memset(recipe, 0, sizeof(recipe));
    memset(packages, 0, sizeof(packages));

    cin >> N >> P;
    for (int i = 0; i < N; i++) {
        cin >> recipe[i];
    }
    for (int n = 0; n < N; n++) {
        amount[n].resize(P);
        for (int p = 0; p < P; p++) {
            cin >> amount[n][p];
            packages[p + n * P] = {n, p};
        }
        sort(begin(amount[n]), end(amount[n]));
    }

    stable_sort(packages, packages + N * P);
    int realized = 0;

    for (int k = 0; k < N * P; k++) {
        int n = packages[k].n;
        int p = packages[k].p;
        if (p < cur[n])
            continue;
        assert(p == cur[n]);
        // 0.9 * servings * recipe <= amount <= 1.1 * servings * recipe
        // 0.9 * servings * recipe <= amount < 0.9 * (servings + 1) * recipe
        // servings <= 10 * amount / (9 * recipe) < servings + 1
        long servings = 10 * amount[n][p] / (9 * recipe[n]);

        // amount <= 1.1 * servings * recipe
        bool ok = true;
        for (int i = 0; ok && i < N; i++) {
            ok = 10 * amount[i][cur[i]] <= 11 * servings * recipe[i];
        }

        if (ok) {
            for (int i = 0; ok && i < N; i++) {
                ok = ++cur[i] < P;
            }
            realized++;
        } else {
            ok = ++cur[n] < P;
        }
        if (!ok) {
            break;
        }
    }
    return realized;
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
