#include <bits/stdc++.h>

using namespace std;

// *****

struct data_t {
    long even = 0, odd = 0, inc_even = 0, inc_odd = 0;
};

struct node_t {
    int L, R;
    data_t data;
};

int N, P;
vector<node_t> tree;

data_t add(const data_t &lhs, const data_t &rhs) {
    return data_t{lhs.even + rhs.even, lhs.odd + rhs.odd,
                  lhs.inc_even + rhs.inc_even, lhs.inc_odd + rhs.inc_odd};
}

void update_data(data_t &data, int i, long v) {
    data.even = (i & 1) ? 0 : v;
    data.odd = (i & 1) ? v : 0;
    data.inc_even = (i & 1) ? 0 : i * v;
    data.inc_odd = (i & 1) ? i * v : 0;
}

// Pushup update at leaf node i
void pushup_update(int i) {
    assert(i >= N);
    while ((i >>= 1) > 0) {
        tree[i].data = add(tree[i << 1].data, tree[i << 1 | 1].data);
    }
}

void update(int ai, long v) {
    int ti = ai < (2 * N - P) ? P + ai : P + ai - N;
    update_data(tree[ti].data, ai, v);
    pushup_update(ti);
}

// Query the sum in the range [L, R] (exclusive in R despite notation)
data_t range_query(int i, int L, int R) {
    auto iL = tree[i].L, iR = tree[i].R;

    // [L, R] overlaps [iL, iR]?
    if (iR <= L || iL >= R)
        return {};
    // [L, R] contains [iL, iR]?
    if (L <= iL && iR <= R)
        return tree[i].data;
    // they only cross
    assert(i < N);
    return add(range_query(i << 1, L, R), range_query(i << 1 | 1, L, R));
}

void pushup_prepare(int i) {
    if (i >= N)
        return;
    pushup_prepare(i << 1);
    pushup_prepare(i << 1 | 1);
    tree[i].L = tree[i << 1].L;
    tree[i].R = tree[i << 1 | 1].R;
    tree[i].data = add(tree[i << 1].data, tree[i << 1 | 1].data);
}

auto solve() {
    int Q;
    cin >> N >> Q;
    P = 1;
    while (P < N)
        P <<= 1;

    vector<int> A(N);
    for (int i = 0; i < N; ++i)
        cin >> A[i];

    // prepare tree
    tree.resize(2 * N);
    for (int i = 0; i < N; ++i) {
        tree[i + N].L = i;
        tree[i + N].R = i + 1;
        update_data(tree[i + N].data, i, A[i]);
    }

    // align leaves to tree
    auto leafbegin = tree.begin() + N, leafend = tree.end();
    rotate(leafbegin, leafbegin + (2 * N - P), leafend);
    pushup_prepare(1);

    auto dt = tree[1].data;

    long sum = 0;

    for (int q = 0; q < Q; ++q) {
        string operation;
        cin >> operation;
        if (operation == "U") {
            int i;
            long v;
            cin >> i >> v;
            // ranges are 0-indexed
            update(i - 1, v);
        }
        if (operation == "Q") {
            int L, R;
            cin >> L >> R;
            // ranges are 0-indexed exclusive end
            data_t data = range_query(1, --L, R);
            auto A = data.even, B = data.odd;
            auto C = data.inc_even, D = data.inc_odd;
            auto value = (C - D - (L - 1) * (A - B));
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
