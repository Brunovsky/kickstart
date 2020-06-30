#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int MAX_COST = 1000000001;

struct range {
    int L, R;
    int cost = MAX_COST;
};

int N, M, L, P;
vector<range> tree;

void pushup() {
    for (int i = L - 1; i > 0; --i) {
        tree[i].L = tree[i << 1].L;
        tree[i].R = tree[i << 1 | 1].R;
    }
}

void update(int l, int cost) {
    int i = l <= (2 * L - P) ? P + l - 1 : P + l - L - 1;
    do {
        if (tree[i].cost <= cost)
            break;
        tree[i].cost = cost;
        i >>= 1;
    } while (i > 0);
}

int query(int l) {
    int i = l <= (2 * L - P) ? P + l - 1 : P + l - L - 1;
    return tree[i].cost;
}

int query_range(int i, int l, int r) {
    // overlap
    if (l <= tree[i].R && tree[i].L <= r) {
        // contain
        if (l <= tree[i].L && tree[i].R <= r) {
            return tree[i].cost;
        } else {
            assert(i < L);
            int min_left = query_range(i << 1, l, r);
            int min_right = query_range(i << 1 | 1, l, r);
            return min(min_left, min_right);
        }
    }

    return MAX_COST;
}

void insert(int a, int b, int c) {
    assert(a <= b);
    for (int i = L; i > b; --i) {
        int lowest = query_range(1, i - b, i - a);
        update(i, lowest + c);
    }
    for (int i = min(b, L); i >= a; --i) {
        update(i, c);
    }
}

auto solve() {
    cin >> N >> M >> L >> ws;
    P = 1;
    while (P < L)
        P <<= 1;

    tree.assign(2 * L, {});
    auto obegin = tree.begin() + L;
    auto oend = tree.end();

    for (int i = 0; i < L; ++i) {
        tree[i + L].L = i + 1;
        tree[i + L].R = i + 1;
    }

    rotate(obegin, obegin + (2 * L - P), oend); // align tree
    pushup();
    for (int i = 0; i < N; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        insert(a, b, c);
    }

    int min_cost = query(L);
    return min_cost <= M ? to_string(min_cost) : "IMPOSSIBLE"s;
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
