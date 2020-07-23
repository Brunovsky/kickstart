#include <bits/stdc++.h>

using namespace std;

// *****

struct data_t {
    long even, odd;
    long ieven, iodd;
};

data_t add(const data_t &lhs, const data_t &rhs) {
    return data_t{lhs.even + rhs.even,   //
                  lhs.odd + rhs.odd,     //
                  lhs.ieven + rhs.ieven, //
                  lhs.iodd + rhs.iodd};
}

void update_data(data_t &data, int i, long v) {
    data.even = (i & 1) ? 0 : v;
    data.odd = (i & 1) ? v : 0;
    data.ieven = (i & 1) ? 0 : i * v;
    data.iodd = (i & 1) ? i * v : 0;
}

struct node_t {
    int L, R;
    data_t data;
};

#define MAXN 200000

int N, P;
node_t tree[MAXN];

// Pushup update at tree leaf node i
void pushup_update(int i) {
    assert(i >= N);
    while ((i >>= 1) > 0) {
        tree[i].data = add(tree[i << 1].data, tree[i << 1 | 1].data);
    }
}

// Update a value at array index i and pushup the update in the tree
void update(int ai, long v) {
    int ti = ai < (2 * N - P) ? P + ai : P + ai - N;
    update_data(tree[ti].data, ai, v);
    pushup_update(ti);
}

// Perform all range queries in the range [L, R[
data_t range_query(int i, int L, int R) {
    if (L < tree[i].R && tree[i].L < R) {
        if (L <= tree[i].L && tree[i].R <= R) {
            return tree[i].data;
        } else {
            assert(i < N);
            return add(range_query(i << 1, L, R), range_query(i << 1 | 1, L, R));
        }
    }
    return {};
}

// Setup the tree after population the array-aligned leaves with data and ranges
void pushup_prepare(int i) {
    if (i < N) {
        pushup_prepare(i << 1);
        pushup_prepare(i << 1 | 1);
        tree[i].L = tree[i << 1].L;
        tree[i].R = tree[i << 1 | 1].R;
        tree[i].data = add(tree[i << 1].data, tree[i << 1 | 1].data);
    }
}

auto solve() {
    int Q;
    cin >> N >> Q;
    P = 1;
    while (P < N) {
        P <<= 1;
    }

    vector<int> A(N);
    for (int i = 0; i < N; ++i) {
        cin >> A[i];
    }

    // prepare tree
    memset(tree, 0, sizeof(tree));
    for (int i = 0; i < N; ++i) {
        tree[i + N].L = i;
        tree[i + N].R = i + 1;
        update_data(tree[i + N].data, i, A[i]);
    }
    rotate(tree + N, tree + (3 * N - P), tree + 2 * N);
    pushup_prepare(1);

    long sum = 0;

    for (int q = 0; q < Q; ++q) {
        string operation;
        cin >> operation;
        if (operation == "U") {
            int i;
            long v;
            cin >> i >> v;
            update(i - 1, v);
        }
        if (operation == "Q") {
            int L, R;
            cin >> L >> R;
            data_t data = range_query(1, --L, R);
            auto a = data.even, b = data.odd;
            auto c = data.ieven, d = data.iodd;
            auto value = (c - d - (L - 1) * (a - b));
            auto query = (L & 1) ? -value : value;
            sum += query;
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
