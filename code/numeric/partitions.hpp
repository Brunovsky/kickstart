#ifndef PARTITIONS_HPP
#define PARTITIONS_HPP

#include "../hash.hpp"
#include "../random.hpp"

/**
 * Number of partitions of n into k parts each of size between m and M.
 * Complexity: Potentially O(n^2)
 */
long count_partitions(int n, int k, int m = 1, int M = INT_MAX) {
    static unordered_map<tuple<int, int, int>, long> cntpt_memo;

    M = min(M, n);
    if (m > 0)
        return count_partitions(n - m * k, k, 0, M - m);
    if (n < 0 || k <= 0 || (n + k - 1) / k > M)
        return 0;
    if (cntpt_memo.count({n, k, M}))
        return cntpt_memo[{n, k, M}];
    if (k == 1)
        return cntpt_memo[{n, k, M}] = n <= M;

    long cnt = 0;
    for (int x = M; x >= m; x--) {
        cnt += count_partitions(n - x, k - 1, m, x);
    }
    return cntpt_memo[{n, k, M}] = cnt;
}

#endif // PARTITIONS_HPP
