#include <bits/stdc++.h>

using namespace std;

// *****

struct range {
    int L, R, hits = 0;
};

long N, M, O, P;
vector<range> tree;

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
    long L1, R1, A, B, C1, C2;
    cin >> N >> L1 >> R1 >> A >> B >> C1 >> C2 >> M;

    vector<range> intervals(N);
    intervals[0] = {int(L1), int(R1)};
    long x = L1, y = R1;
    for (int i = 1; i < N; ++i) {
        long xx = (A * x + B * y + C1) % M;
        long yy = (A * y + B * x + C2) % M;
        x = xx, y = yy;
        intervals[i] = {int(min(x, y)), int(max(x, y))};
    }

    vector<int> endp;
    endp.reserve(2 * N);
    for (const auto &interval : intervals) {
        endp.push_back(interval.L);
        endp.push_back(interval.R + 1);
    }
    sort(begin(endp), end(endp));
    endp.erase(unique(begin(endp), end(endp)), end(endp));
    O = endp.size() - 1;
    P = 1;
    while (P < O) {
        P <<= 1;
    }

    tree.assign(2 * O, {});

    for (int i = 0; i < O; ++i) {
        tree[i + O].L = endp[i];
        tree[i + O].R = endp[i + 1];
    }

    rotate(begin(tree) + O, begin(tree) + (3 * O - P), end(tree)); // align tree
    for (int i = O - 1; i > 0; --i) {
        tree[i].L = tree[i << 1].L;
        tree[i].R = tree[i << 1 | 1].R;
    }
    for (auto interval : intervals) {
        insert(1, interval.L, interval.R + 1);
    }
    for (int i = 1; i < O; ++i) {
        tree[i << 1].hits += tree[i].hits;
        tree[i << 1 | 1].hits += tree[i].hits;
    }
    rotate(begin(tree) + O, begin(tree) + P, end(tree)); // align array

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
        int i = lower_bound(begin(endp), end(endp), L) - begin(endp);
        int j = lower_bound(begin(endp), end(endp), R) - begin(endp);
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
