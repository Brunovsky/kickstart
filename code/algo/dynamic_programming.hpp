#ifndef DYNAMIC_PROGRAMMING_HPP
#define DYNAMIC_PROGRAMMING_HPP

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

#endif // DYNAMIC_PROGRAMMING_HPP
