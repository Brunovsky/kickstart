#ifndef LYNDON_HPP
#define LYNDON_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Build the lyndon factorization of s=w1w2...wk such that all wi are simple and
 * w1>=w2>=...>=wk. A word is simple if it is strictly smallest than all its suffixes
 * and cyclic shifts.
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/lyndon_factorization.html
 */
template <typename Vec>
vector<int> build_lyndon_factorization(const Vec& s) {
    int N = s.size(), i = 0;
    vector<int> words;
    while (i < N) {
        int j = i + 1, k = i;
        while (j < N && s[k] <= s[j])
            s[k] < s[j] ? k = i : k++, j++;
        while (i <= k)
            words.push_back(i), i += j - k;
    }
    return words;
}

/**
 * Compute minimum rotation of string/vec s
 *
 * Complexity: O(N)
 */
template <typename Vec>
int min_cyclic_string(Vec s) {
    s.reserve(2 * s.size());
    s.insert(end(s), begin(s), end(s));
    int N = s.size(), i = 0, ans = 0;
    while (i < N / 2) {
        ans = i;
        int j = i + 1, k = i;
        while (j < N && s[k] <= s[j])
            s[k] < s[j] ? k = i : k++, j++;
        while (i <= k)
            i += j - k;
    }
    return ans;
}

#endif // LYNDON_HPP
