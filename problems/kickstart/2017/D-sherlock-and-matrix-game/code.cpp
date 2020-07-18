#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
namespace gnu = __gnu_pbds;

using min_tree =
    gnu::tree<pair<int, int>, gnu::null_type, less<pair<int, int>>,
              gnu::rb_tree_tag, gnu::tree_order_statistics_node_update>;

// *****

#define MAXSUM 100000073L
#define MAXPROD (MAXSUM * MAXSUM)

void read(long &N, long &K, vector<long> &A, vector<long> &B) {
    long A1, B1, C, D, E1, E2, F;
    cin >> N >> K >> A1 >> B1 >> C >> D >> E1 >> E2 >> F >> ws;
    A.resize(N, 0);
    B.resize(N, 0);
    A[0] = A1, B[0] = B1;
    long x1 = A1, y1 = B1, r1 = 0, s1 = 0;
    for (long i = 1; i < N; i++) {
        long xi = (C * x1 + D * y1 + E1) % F;
        long yi = (D * x1 + C * y1 + E2) % F;
        long ri = (C * r1 + D * s1 + E1) & 1;
        long si = (D * r1 + C * s1 + E2) & 1;
        A[i] = ri ? -xi : xi;
        B[i] = si ? -yi : yi;
        x1 = xi, y1 = yi, r1 = ri, s1 = si;
    }
}

// count subarray sums >= n
auto count_sums(const vector<long> &V, int n) {
    min_tree tree;
    tree.insert({0, 0});
    long count = 0;
    long sum = 0;
    int L = V.size();
    for (int i = 0; i < L; i++) {
        // sum - prefix >= n  <==>  prefix <= sum - n
        sum += V[i];
        count += tree.order_of_key({sum - n, LONG_MAX});
        tree.insert({sum, i + 1});
    }
    return count;
}

// generate subarray sums >= n, up to K of them
auto gen_sums(const vector<long> &V, int n, int K) {
    min_tree tree;
    tree.insert({0, 0});
    vector<long> sums;
    sums.reserve(K);
    long sum = 0;
    int L = V.size();
    for (long i = 0; i < L; i++) {
        sum += V[i];
        // sum - prefix >= n
        for (auto p : tree) {
            long subsum = sum - p.first;
            if (n >= subsum)
                break;
            sums.push_back(subsum);
        }
        tree.insert({sum, i + 1});
    }
    assert(long(sums.size()) <= K);
    sums.resize(K, n);
    sort(begin(sums), end(sums));
    return sums;
}

// directly generate all subarray sums
auto gen_all_sums(const vector<long> &V) {
    vector<long> sums;
    long L = V.size();
    sums.reserve(L * (L - 1) / 2);
    for (long i = 0; i < L; i++) {
        long sum = 0;
        for (long j = i; j < L; j++) {
            sum += V[j];
            sums.push_back(sum);
        }
    }
    sort(begin(sums), end(sums));
    return sums;
}

long kth_largest_sum(const vector<long> &V, long K) {
    long L = -MAXSUM;
    long R = MAXSUM;
    do {
        long m = (L + R) / 2;
        (count_sums(V, m) >= K) ? L = m + 1 : R = m - 1;
    } while (L <= R);
    return L;
}

void sym(vector<long> &V) {
    for (auto &num : V) {
        num = -num;
    }
}

auto find_and_gen(vector<long> &V, long K) {
    long v = V.size();
    if (4 * K >= v * (v - 1)) {
        return gen_all_sums(V);
    }

    long kth_largest = kth_largest_sum(V, K);
    auto largest_sums = gen_sums(V, kth_largest, K);
    sym(V);
    long kth_smallest = kth_largest_sum(V, K);
    auto smallest_sums = gen_sums(V, kth_smallest, K);
    sym(V);
    sym(smallest_sums);
    reverse(smallest_sums.begin(), smallest_sums.end());

    smallest_sums.resize(2 * K);
    for (long i = 0; i < K; i++) {
        smallest_sums[i + K] = largest_sums[i];
    }
    return smallest_sums;
}

long kth(long K, const vector<long> &sA, const vector<long> &sB) {
    long a0i = lower_bound(begin(sA), end(sA), 0) - begin(sA);
    long a1i = lower_bound(begin(sA), end(sA), 1) - begin(sA);
    long b0i = lower_bound(begin(sB), end(sB), 0) - begin(sB);
    long b1i = lower_bound(begin(sB), end(sB), 1) - begin(sB);
    long a = sA.size(), b = sB.size();

    long azeros = a1i - a0i, bzeros = b1i - b0i;
    long zeros = b * azeros + a * bzeros - azeros * bzeros;

    const long al[4] = {a1i, a - 1, 0, a0i - 1};
    const long ar[4] = {a - 1, a1i, a0i - 1, 0};
    const long bl[4] = {b - 1, b0i - 1, b1i, 0};
    const long br[4] = {b1i, 0, b - 1, b0i - 1};

    long pmin = -MAXPROD, pmax = MAXPROD;
    while (pmin <= pmax) {
        long p = (pmin + pmax) / 2;
        long count = 0;

        // fuck these annoying zeros
        if (p <= 0) {
            count += zeros;
        }

        for (int sa : {0, 1}) {
            for (int sb : {0, 1}) {
                int n = 2 * sa + sb;
                const int da = sb ? -1 : 1;
                const int db = sa ? 1 : -1;
                int ai = al[n], amax = ar[n];
                int bi = bl[n], bmax = br[n];
                int k = 0;
                while (da * (amax - ai) >= 0) {
                    while (db * (bmax - bi) >= 0 && sA[ai] * sB[bi] >= p) {
                        bi += db;
                        k++;
                    }
                    count += k;
                    ai += da;
                }
            }
        }

        if (count >= K) {
            pmin = p + 1;
        } else {
            pmax = p - 1;
        }
    }

    return pmin - 1;
}

auto solve() {
    long N, K;
    vector<long> A;
    vector<long> B;
    read(N, K, A, B);

    auto sA = find_and_gen(A, K);
    auto sB = find_and_gen(B, K);

    return kth(K, sA, sB);
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
