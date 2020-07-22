#include <bits/stdc++.h>

using namespace std;

// *****

#define MAX 200'001
long double fac[MAX];

long W, H, L, R, U, D;

void prepare() {
    fac[0] = fac[1] = 0;
    for (int i = 2; i < MAX; i++) {
        long double n = i;
        fac[i] = fac[i - 1] + log2(n);
    }
}

long double choose(int n, int k) {
    long double e = fac[n] - fac[k] - fac[n - k] - n;
    return exp2(e);
}

long double under(int L, int D, int H) {
    int n = L + D;
    if (L == 0 || D == H) {
        return 0.0;
    }
    long double sum = 0.0;
    for (int k = 0; k < L; k++) {
        sum += choose(n, k);
    }
    return sum;
}

auto solve() {
    cin >> W >> H >> L >> U >> R >> D >> ws;
    --W, --H, --L, --U, --R, --D;
    return under(L, D, H) + under(U, R, W);
}

// *****

int main() {
    prepare();
    unsigned T;
    cout << showpoint << fixed << setprecision(8);
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        auto solution = solve();
        cout << "Case #" << t << ": " << solution << '\n';
    }
    return 0;
}
