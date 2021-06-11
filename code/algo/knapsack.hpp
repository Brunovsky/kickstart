#pragma once

#include <bits/stdc++.h>
using namespace std;

/**
 * Determine whether there exists a subset of nums with the given sum.
 * O(ns) time, where s is the number of unique sums.
 * Returns the lexicographically smallest assignment
 */
template <typename T>
optional<vector<int>> sparse_subset_sum(const vector<T>& nums, T target) {
    int N = nums.size();
    map<T, int> seen;
    seen[T(0)] = -1;

    for (int i = 0; i < N && !seen.count(target); i++) {
        auto x = nums[i];
        if (x > 0) {
            for (auto it = seen.rbegin(); it != seen.rend(); ++it) {
                seen.emplace(it->first + x, i);
            }
        } else if (x < 0) {
            for (auto it = seen.begin(); it != seen.end(); ++it) {
                seen.emplace(it->first + x, i);
            }
        }
    }

    if (seen.count(target)) {
        vector<int> indices;
        while (target != 0) {
            int i = seen.at(target);
            indices.push_back(i);
            target -= nums[i];
        }
        reverse(begin(indices), end(indices));
        return indices;
    } else {
        return std::nullopt;
    }
}

optional<vector<int>> dense_subset_sum(const vector<int>& nums, int target) {
    int N = nums.size();
    int neg = 0, pos = 0;
    for (int i = 0; i < N; i++) {
        if (nums[i] > 0) {
            pos += nums[i];
        } else {
            neg += nums[i];
        }
    }
    if (target < neg || pos < target) {
        return std::nullopt;
    }

    int S = pos - neg + 1;
    assert(S <= 75'000'000); // sanity check

    vector<int> seen(S, 0); // seen[x + neg] for x
    seen[0 - neg] = -1;

    for (int i = 0; i < N && !seen[target - neg]; i++) {
        auto x = nums[i];
        if (x > 0) {
            for (int s = pos; s >= x + neg; s--) {
                if (!seen[s - neg] && seen[s - x - neg]) {
                    seen[s - neg] = i + 1;
                }
            }
        } else if (x < 0) {
            for (int s = neg; s <= x + pos; s++) {
                if (!seen[s - neg] && seen[s - x - neg]) {
                    seen[s - neg] = i + 1;
                }
            }
        }
    }

    if (seen[target - neg]) {
        vector<int> indices;
        while (target != 0) {
            int i = seen[target - neg] - 1;
            indices.push_back(i);
            target -= nums[i];
        }
        reverse(begin(indices), end(indices));
        return indices;
    } else {
        return std::nullopt;
    }
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
    return make_pair(total, quantity);
}

/**
 * Maximum value 0-1 knapsack
 * O(nW) time, O(n*cap) space
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
    return make_pair(total, quantity);
}
