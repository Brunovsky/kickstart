#ifndef KNAPSACK_HPP
#define KNAPSACK_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Whether there exists a subset with target sum
 * O(ns) time, where s is the total range of sums.
 */
bool subset_sum(const vector<int>& nums, int target) {
    int N = nums.size();
    int neg = 0, pos = 0;
    for (int i = 0; i < N; i++)
        nums[i] > 0 ? pos += nums[i] : neg += nums[i];

    if (target < neg || pos < target)
        return false;

    // note: if you have a dynamic bitset class, use it instead and shift by x
    int S = pos - neg + 1;
    vector<bool> dp(S, false);

    for (int i = 0; i < N; i++) {
        int x = nums[i];
        if (x >= 0) {
            for (int s = pos; s >= x + neg; s--) {
                dp[s - neg] = dp[s - neg] | dp[s - x - neg];
            }
        } else {
            for (int s = neg; s <= x + pos; s++) {
                dp[s - neg] = dp[s - neg] | dp[s - x - neg];
            }
        }
        dp[x - neg] = true;
        if (dp[target - neg])
            return true;
    }

    return false;
}

/**
 * Maximum value repeated knapsack
 * O(nW) time, O(cap) space
 */
auto repeated_knapsack(int cap, const vector<int>& weight, const vector<int>& value) {
    int N = weight.size();
    vector<int> dp(cap + 1, 0);
    vector<int> pred(cap + 1, -1);

    for (int w = 1; w <= cap; w++) {
        for (int i = 0; i < N; i++) {
            if (weight[i] <= w && dp[w] < dp[w - weight[i]] + value[i]) {
                dp[w] = dp[w - weight[i]] + value[i];
                pred[w] = i;
            }
        }
    }

    int total = 0, w = cap;
    vector<int> quantity(N, 0);
    while (pred[w] != -1) {
        total += value[pred[w]];
        quantity[pred[w]]++;
        w -= weight[pred[w]];
    }
    return pair<int, vector<int>>{total, quantity};
}

/**
 * Maximum value 0-1 knapsack
 * O(nW) time, O(cap) space
 */
auto unit_knapsack(int cap, const vector<int>& weight, const vector<int>& value) {
    int N = weight.size();
    vector<vector<int>> dp(N + 1, vector<int>(cap + 1, 0));

    for (int i = 0; i < N; i++) {
        for (int w = 0; w < weight[i]; w++) {
            dp[i + 1][w] = dp[i][w];
        }
        for (int w = weight[i]; w <= cap; w++) {
            dp[i + 1][w] = max(dp[i][w], dp[i][w - weight[i]] + value[i]);
        }
    }

    int total = 0;
    vector<bool> quantity(N, false);
    for (int w = cap, i = N - 1; w && i >= 0; i--) {
        if (dp[i + 1][w] != dp[i][w]) {
            quantity[i] = true;
            total += value[i];
            w -= weight[i];
        }
    }
    return pair<int, vector<bool>>{total, quantity};
}

#endif // KNAPSACK_HPP
