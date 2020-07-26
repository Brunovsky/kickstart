#include <bits/stdc++.h>

using namespace std;

// *****

#define MISMATCH(i, j, len)                                                      \
    mismatch(begin(W[i]) + len, end(W[i]), begin(W[j]) + len, end(W[j])).first - \
        begin(W[i])

#define MAXN 1000
#define MAXL 50

int N;
string W[MAXN];

struct Count {
    int one = 0, two = 0;
};

Count subdivide(int i, int j, int len) {
    assert(i + 1 < j);
    Count cnt;
    int k = i;
    while (k + 1 < j) {
        int s = k;
        int e = k + 1;
        int sublen = MISMATCH(s, e, len);
        if (sublen == len) {
            cnt.one++;
            k = k + 1;
        } else {
            e++;
            while (e < j && MISMATCH(s, e, len) > len) {
                e++;
            }
            Count sub = subdivide(s, e, len + 1);
            cnt.one += sub.one, cnt.two += sub.two;
            k = e;
        }
    }
    if (k < j) {
        cnt.one++;
    }
    if (len > 0 && cnt.one >= 2) {
        cnt.one -= 2;
        cnt.two += 2;
    }
    return cnt;
}

auto solve() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> W[i];
        reverse(begin(W[i]), end(W[i]));
    }
    sort(W, W + N);
    return subdivide(0, N, 0).two;
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
