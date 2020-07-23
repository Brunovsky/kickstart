#include <bits/stdc++.h>

using namespace std;

// *****

struct node_t {
    long hi, lo;
    long classes;
    long prefix;
};

#define MAXN 400'005
#define MAXO 800'010
#define MAXQ 100'005

long N, Q;
int O, P;
node_t tree[4 * MAXN];
long endp[MAXO];
long Hi[MAXN];
long Lo[MAXN];
long K[MAXQ];

void add_class(long hi, long lo, int i) {
    // overlap
    if (hi > tree[i].lo && tree[i].hi > lo) {
        // contain
        if (hi >= tree[i].hi && tree[i].lo >= lo) {
            ++tree[i].classes;
        } else {
            assert(i < O);
            add_class(hi, lo, i << 1);
            add_class(hi, lo, i << 1 | 1);
        }
    }
}

void pushdown_classes(int i) {
    tree[i].classes += tree[i >> 1].classes;
    if (i < O) {
        pushdown_classes(i << 1);
        pushdown_classes(i << 1 | 1);
    }
}

bool node_cmp(const node_t &node, long k) {
    return node.prefix < k;
}

void read() {
    cin >> N >> Q >> ws;
    long x1, x2, x3, a1, b1, c1, m1;
    long y1, y2, y3, a2, b2, c2, m2;
    long z1, z2, z3, a3, b3, c3, m3;
    cin >> x1 >> x2 >> a1 >> b1 >> c1 >> m1 >> ws;
    cin >> y1 >> y2 >> a2 >> b2 >> c2 >> m2 >> ws;
    cin >> z1 >> z2 >> a3 >> b3 >> c3 >> m3 >> ws;

    Hi[0] = max(x1, y1) + 1;
    Lo[0] = min(x1, y1);
    Hi[1] = max(x2, y2) + 1;
    Lo[1] = min(x2, y2);

    for (long i = 2; i < N; ++i) {
        x3 = (a1 * x2 + b1 * x1 + c1) % m1;
        y3 = (a2 * y2 + b2 * y1 + c2) % m2;
        x1 = x2;
        x2 = x3;
        y1 = y2;
        y2 = y3;
        Hi[i] = max(x3, y3) + 1;
        Lo[i] = min(x3, y3);
    }

    K[0] = z1 + 1;
    K[1] = z2 + 1;

    for (long i = 2; i < Q; ++i) {
        z3 = (a3 * z2 + b3 * z1 + c3) % m3;
        z1 = z2;
        z2 = z3;
        K[i] = z3 + 1;
    }
}

auto solve() {
    read();
    memset(tree, 0, sizeof(tree));
    memset(endp, 0, sizeof(endp));

    for (int i = 0; i <= N; i++) {
        endp[i << 1] = Hi[i];
        endp[i << 1 | 1] = Lo[i];
    }
    sort(endp, endp + 2 * N + 2, greater<long>{});
    O = unique(endp, endp + 2 * N + 2) - endp - 1;
    P = 1;
    while (P < O) {
        P <<= 1;
    }

    for (int i = O, j = 0; i < 2 * O; ++i, ++j) {
        tree[i].hi = endp[j];
        tree[i].lo = endp[j + 1];
    }

    rotate(tree + O, tree + (3 * O - P), tree + 2 * O);
    for (int i = O - 1; i > 0; i--) {
        tree[i].hi = tree[i << 1].hi;
        tree[i].lo = tree[i << 1 | 1].lo;
    }
    for (long n = 0; n < N; ++n) {
        add_class(Hi[n], Lo[n], 1);
    }
    pushdown_classes(1);
    rotate(tree + O, tree + P, tree + 2 * O);

    for (int i = O; i < 2 * O; ++i) {
        long hi = tree[i].hi, lo = tree[i].lo, c = tree[i].classes;
        tree[i].prefix = (hi - lo) * c + tree[i - 1].prefix;
    }

    long sum = 0;

    for (long q = 1; q <= Q; ++q) {
        long k = K[q - 1];
        int i = lower_bound(tree + O, tree + 2 * O, k, node_cmp) - tree;
        if (i < 2 * O) {
            long a = tree[i - 1].prefix;
            long c = tree[i].classes;
            long hi = tree[i].hi;
            long S = hi - (k - a - 1) / c;
            sum += S * q;
        }
    }

    return sum;
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
