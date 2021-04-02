#include <bits/stdc++.h>

using namespace std;

// *****

#define ALL(s) begin(s), end(s)
#define MAXV   100

struct segtree {
    struct node_t {
        int L, R;
        int sum = 0;
        vector<int> sums[2];
        long sumsum[2];
    };

    int N, Q;
    vector<int> A;
    vector<node_t> tree;
    unordered_map<long, long> count_cache, sum_cache;

    long count(int i, int S) {
        if (i >= N) {
            return tree[i].sum <= S;
        }
        long many = 0;
        int S0 = tree[i].sums[0].size(), S1 = tree[i].sums[1].size();
        for (int l = 0, r = 0; l < S0 && r < S1; l++) {
            while (r < S1 && tree[i].sums[0][l] + tree[i].sums[1][r] > S)
                r++;
            many += S1 - r;
        }
        return many + count(i << 1, S) + count(i << 1 | 1, S);
    }

    // number of subarrays with sum <= S.
    long count(int S) {
        return count_cache.count(S) ? count_cache[S] : (count_cache[S] = count(1, S));
    }

    long sum(int i, int S) {
        if (i >= N) {
            return tree[i].sum <= S ? tree[i].sum : 0;
        }
        long many = 0;
        int S0 = tree[i].sums[0].size(), S1 = tree[i].sums[1].size();
        long right = tree[i].sumsum[1];
        for (int l = 0, r = 0; l < S0 && r < S1; l++) {
            while (r < S1 && tree[i].sums[0][l] + tree[i].sums[1][r] > S)
                right -= tree[i].sums[1][r++];
            many += 1L * (S1 - r) * tree[i].sums[0][l] + right;
        }
        return many + sum(i << 1, S) + sum(i << 1 | 1, S);
    }

    // sum of all subarray sums of value <= S.
    long sum(int S) {
        return sum_cache.count(S) ? sum_cache[S] : (sum_cache[S] = sum(1, S));
    }

    // largest S such that there are <=L subarray sums of value <=S.
    int lower_bound(long L) {
        int lo = 0, hi = N * MAXV;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (count(mid) <= L) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        return lo - 1;
    }

    long query(long L, long R) {
        long bound_L = lower_bound(L - 1), bound_R = lower_bound(R);
        long count_L = count(bound_L), count_R = count(bound_R);
        long sum_L = sum(bound_L), sum_R = sum(bound_R);

        long upper = sum_R + (bound_R + 1) * (R - count_R);
        long lower = sum_L + (bound_L + 1) * (L - count_L - 1);
        return upper - lower;
    }

    segtree(const vector<int>& A) : A(A) {
        N = A.size(), Q = 1;
        tree.assign(2 * N, {});
        while (Q < N)
            Q <<= 1;
        for (int i = 0; i < N; ++i) {
            tree[i + N].L = i;
            tree[i + N].R = i + 1;
            tree[i + N].sum = A[i];
            tree[i + N].sums[0] = tree[i + N].sums[1] = {A[i]};
        }
        rotate(begin(tree) + N, begin(tree) + (3 * N - Q), begin(tree) + 2 * N);
        for (int i = N - 1; i > 0; i--) {
            tree[i].L = tree[i << 1].L;
            tree[i].R = tree[i << 1 | 1].R;
        }
        for (int i = N - 1; i > 0; i--) {
            int l = i << 1, r = i << 1 | 1;
            tree[i].sum = tree[l].sum + tree[r].sum;
            for (int j = tree[i << 1].R - 1, s = 0; j >= tree[i].L; j--)
                tree[i].sums[0].push_back(s += A[j]);
            for (int j = tree[i << 1].R, s = 0; j < tree[i].R; j++)
                tree[i].sums[1].push_back(s += A[j]);
            reverse(ALL(tree[i].sums[1]));
            tree[i].sumsum[0] = accumulate(ALL(tree[i].sums[0]), 0L);
            tree[i].sumsum[1] = accumulate(ALL(tree[i].sums[1]), 0L);
        }
    }
};

auto solve() {
    int N, Q;
    cin >> N >> Q;
    vector<int> A(N);
    for (int i = 0; i < N; i++)
        cin >> A[i];

    segtree tree(A);

    for (int q = 0; q < Q; q++) {
        long L, R;
        cin >> L >> R;
        cout << tree.query(L, R) << endl;
    }
}

// *****

int main() {
    unsigned T;
    cin >> T >> ws;
    for (unsigned t = 1; t <= T; ++t) {
        cout << "Case #" << t << ":\n";
        solve();
    }
    return 0;
}
