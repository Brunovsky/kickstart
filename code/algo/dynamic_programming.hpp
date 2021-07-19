#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Longest increasing subsequence of one list
 * O(n log n) time, O(n) space
 * For non-decreasing, change nums[M[mid]] < nums[i] to <=.
 */
template <typename T, typename CmpFn = less<T>>
vector<int> longest_increasing_subsequence(const vector<T>& nums, CmpFn&& cmp = CmpFn()) {
    int N = nums.size();
    vector<int> P(N, 0), M(N + 1, 0);

    int L = 0;
    for (int i = 0; i < N; i++) {
        int lo = 1, hi = L;
        while (lo <= hi) {
            int mid = (lo + hi + 1) / 2;
            if (cmp(nums[M[mid]], nums[i]))
                lo = mid + 1;
            else
                hi = mid - 1;
        }

        P[i] = M[lo - 1];
        M[lo] = i;
        L = max(L, lo);
    }

    vector<T> subsequence(L);
    for (int i = L - 1, k = M[L]; i >= 0; i--) {
        subsequence[i] = nums[k];
        k = P[k];
    }
    return subsequence;
}

/**
 * Longest common subsequence of two strings a and b
 * O(ab) time, O(ab) memory to recover subsequence
 */
template <typename Vec>
int longest_common_subsequence_length(const Vec& a, const Vec& b) {
    int A = a.size(), B = b.size();
    vector<int> next(B + 1, 0), prev(B + 1, 0);

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if (a[i] == b[j]) {
                next[j + 1] = 1 + prev[j];
            } else {
                next[j + 1] = max(next[j], prev[j + 1]);
            }
        }
        swap(next, prev);
    }

    return prev[B];
}

template <typename Vec>
Vec longest_common_subsequence(const Vec& a, const Vec& b) {
    int A = a.size(), B = b.size();
    vector<vector<int>> dp(A + 1, vector<int>(B + 1, 0));

    for (int i = 0; i < A; i++) {
        for (int j = 0; j < B; j++) {
            if (a[i] == b[j])
                dp[i + 1][j + 1] = 1 + dp[i][j];
            else
                dp[i + 1][j + 1] = max(dp[i + 1][j], dp[i][j + 1]);
        }
    }

    Vec subsequence;
    int i = A - 1, j = B - 1;
    while (i >= 0 && j >= 0 && dp[i + 1][j + 1]) {
        if (a[i] == b[j]) {
            subsequence.push_back(a[i]), i--, j--;
        } else if (dp[i + 1][j + 1] == dp[i][j + 1]) {
            i--;
        } else {
            j--;
        }
    }
    reverse(begin(subsequence), end(subsequence));
    return subsequence;
}

/**
 * Compute minimum number of intervals [ai,bi) to cover [a,b) (greedy)
 * Complexity: O(N log N)
 */
template <typename T>
optional<vector<int>> minimum_interval_cover(T a, T b, const vector<array<T, 2>>& intv) {
    if (a >= b) {
        return vector<int>();
    }

    int N = intv.size();
    vector<int> index(N);
    iota(begin(index), end(index), 0);
    sort(begin(index), end(index), [&](int u, int v) { return intv[u] < intv[v]; });

    T R = a;
    int i = 0;
    priority_queue<pair<T, int>> pq;
    vector<int> solution;

    while (R < b) {
        while (i < N && intv[index[i]][0] <= R) {
            pq.push({intv[index[i]][1], index[i]}), i++;
        }
        if (pq.empty() || pq.top().first <= R) {
            return std::nullopt;
        }
        auto [r, best] = pq.top();
        pq.pop();
        R = r;
        solution.push_back(best);
    }

    return solution;
}
