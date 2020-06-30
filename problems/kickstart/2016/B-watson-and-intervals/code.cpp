#include <bits/stdc++.h>

using namespace std;

// *****

using i64 = int64_t;

struct range {
    int L, R, hits = 0;
};

i64 N, M, O, P;
vector<range> tree;

void pushup() {
    for (int i = O - 1; i > 0; --i) {
        tree[i].L = tree[i << 1].L;
        tree[i].R = tree[i << 1 | 1].R;
    }
}

void pushdown() {
    for (int i = 1; i < O; ++i) {
        tree[i << 1].hits += tree[i].hits;
        tree[i << 1 | 1].hits += tree[i].hits;
    }
}

void insert(int i, int L, int R) {
    // overlap
    if (L < tree[i].R && tree[i].L < R) {
        // contain
        if (L <= tree[i].L && tree[i].R <= R) {
            tree[i].hits++;
        } else {
            assert(i < O);
            insert(i << 1, L, R);
            insert(i << 1 | 1, L, R);
        }
    }
}

auto solve() {
    i64 L1, R1, A, B, C1, C2;
    cin >> N >> L1 >> R1 >> A >> B >> C1 >> C2 >> M;

    vector<range> intervals(N);
    intervals[0] = {L1, R1};
    i64 x = L1, y = R1;
    for (int i = 1; i < N; ++i) {
        i64 xx = (A * x + B * y + C1) % M;
        i64 yy = (A * y + B * x + C2) % M;
        x = xx, y = yy;
        intervals[i] = {min(x, y), max(x, y)};
    }

    vector<int> endp;
    endp.reserve(2 * N);
    for (const auto &interval : intervals) {
        endp.push_back(interval.L);
        endp.push_back(interval.R + 1);
    }
    sort(endp.begin(), endp.end());
    endp.erase(unique(endp.begin(), endp.end()), endp.end());
    O = endp.size() - 1;
    P = 1;
    while (P < O)
        P <<= 1;

    tree.assign(2 * O, {});
    auto obegin = tree.begin() + O;
    auto oend = tree.end();

    for (int i = 0; i < O; ++i) {
        tree[i + O].L = endp[i];
        tree[i + O].R = endp[i + 1];
    }

    rotate(obegin, obegin + (2 * O - P), oend); // align tree
    pushup();
    for (auto interval : intervals) {
        insert(1, interval.L, interval.R + 1);
    }
    pushdown();
    rotate(obegin, obegin + (P - O), oend); // align array

    // repurpose hits counter
    tree[O - 1].hits = 0;
    int covered = 0;
    for (int i = O; i < 2 * O; ++i) {
        int len = tree[i].R - tree[i].L;
        if (tree[i].hits) {
            covered += len;
        }
        if (tree[i].hits == 1) {
            tree[i].hits = tree[i - 1].hits + (tree[i].R - tree[i].L);
        } else {
            tree[i].hits = tree[i - 1].hits;
        }
    }

    int uncovered = 0;
    for (auto interval : intervals) {
        int L = interval.L, R = interval.R + 1;
        int i = lower_bound(endp.begin(), endp.end(), L) - endp.begin();
        int j = lower_bound(endp.begin(), endp.end(), R) - endp.begin();
        int uniq = tree[j + O - 1].hits - tree[i + O - 1].hits;
        uncovered = max(uncovered, uniq);
    }
    return covered - uncovered;
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
