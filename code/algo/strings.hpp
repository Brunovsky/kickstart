#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Compute the suffix array of a string.
 * sa[i]: Starting index of the rotation in the (i+1)th lexicographical order.
 *
 * You should append to s a value smaller than any value in s and then pop the front
 * element of the returned SA. If not, the algorithm computes a sorting of rotations
 * instead of suffixes, breaking ties arbitrarily.
 * To get the LCP as well do so before popping SA[0], and then pop LCP[0] as well.
 *
 * Complexity: O(N log N)
 * Reference: https://cp-algorithms.com/string/suffix-array.html
 */
template <typename Vec, typename T = typename Vec::value_type>
auto build_cyclic_shifts(const Vec& s, int A, T first) {
    int N = s.size();
    if (N == 0)
        return vector<int>{};

    int C = 0, S = 1; // #classes - 1, sorted width
    vector<int> cnt(max(A, N), 0), sa(N, 0), clazz(N, 0), pn(N, 0), clazzn(N, 0);

    for (int i = 0; i < N; i++)
        cnt[s[i] - first]++;
    for (int a = 1; a < A; a++)
        cnt[a] += cnt[a - 1];
    for (int i = N - 1; i >= 0; i--)
        sa[--cnt[s[i] - first]] = i;
    for (int i = 1; i < N; i++)
        clazz[sa[i]] = C += s[sa[i]] != s[sa[i - 1]];

    while (S < N) {
        for (int i = 0; i < N; i++)
            pn[i] = sa[i] - S, pn[i] += pn[i] < 0 ? N : 0;
        for (int c = 0; c <= C; c++)
            cnt[c] = 0;
        for (int i = 0; i < N; i++)
            cnt[clazz[pn[i]]]++;
        for (int a = 1; a <= C; a++)
            cnt[a] += cnt[a - 1];
        for (int i = N - 1; i >= 0; i--) // backwards for stable sort
            sa[--cnt[clazz[pn[i]]]] = pn[i];

        if (2 * S >= N)
            break;

        clazzn[sa[0]] = C = 0;
        for (int i = 1; i < N; i++) {
            int j = sa[i] + S, k = sa[i - 1] + S;
            j -= j >= N ? N : 0, k -= k >= N ? N : 0;
            clazzn[sa[i]] = C += clazz[k] != clazz[j] || clazz[sa[i]] != clazz[sa[i - 1]];
        }
        swap(clazz, clazzn);
        S *= 2;
    }

    return sa;
}

/**
 * Compute the LCP array for string s and its suffix/cyclic shifts array.
 * LCP[i] = longest common prefix(sa[i], sa[i+1]) for i=0,...,N.
 *
 * Complexity: O(N)
 */
template <typename Vec>
auto build_lcp_array(const Vec& s, const vector<int>& sa) {
    int N = s.size();
    vector<int> rank(N, 0), lcp(N, 0);
    for (int i = 1; i < N; i++) {
        rank[sa[i]] = i;
    }
    for (int i = 0, len = 0; i < N; lcp[rank[i++]] = len, len && len--) {
        if (rank[i] + 1 < N) {
            int j = sa[rank[i] + 1] + len, k = i + len;
            j -= j >= N ? N : 0, k -= k >= N ? N : 0;
            while (len < N && s[j] == s[k])
                len++, j++, k++, j -= j >= N ? N : 0, k -= k >= N ? N : 0;
        }
    }
    if (N > 0) {
        lcp[N - 1] = s[sa[0]] == s[sa[N - 1]] ? N : 0; // wrap around
    }
    return lcp;
}

/**
 * Compute the z function of string s.
 * z[i] := longest common prefix of s[0..] and s[i..]; z[i]=0.
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/z-function.html
 */
template <typename Vec>
auto build_z_function(const Vec& s) {
    int N = s.size();
    vector<int> z(N);
    for (int i = 1, l = 0, r = 0; i < N; i++) {
        if (i <= r)
            z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < N && s[z[i]] == s[i + z[i]])
            z[i]++;
        if (i + z[i] > r + 1)
            l = i, r = i + z[i] - 1;
    }
    return z;
}

/**
 * Compute the prefix function of string s (KMP function)
 * pf[i] := longest proper prefix of s[0..i] that is also a suffix of it; pf[0]=0.
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/prefix-function.html
 */
template <typename Vec>
auto build_prefix_function(const Vec& s) {
    int N = s.size();
    vector<int> pf(N);
    for (int i = 1; i < N; i++) {
        int j = pf[i - 1];
        while (j > 0 && s[i] != s[j])
            j = pf[j - 1];
        if (s[i] == s[j])
            j++;
        pf[i] = j;
    }
    return pf;
}

/**
 * Build the lyndon factorization of s=w1w2...wk such that all wi are simple and
 * w1>=w2>=...>=wk. A word is simple if it is strictly smallest than all its suffixes
 * and cyclic shifts.
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/lyndon_factorization.html
 */
template <typename Vec>
auto build_lyndon_factorization(const Vec& s) {
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
 * Compute minimum cyclic rotation of string s.
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

/**
 * Compute the longest palindrome lp[i] around position i for string s.
 * lp[0][i]: number of palindromes around s[i-1|i]; lp[0][0]=0, lp[0][i]>=0.
 *           biggest starts at s[i-lp[0][i]], ends at s[i-1+lp[0][i]].
 * lp[1][i]: number of palindromes around s[i]; lp[1][i]>=1.
 *           biggest starts at s[i+1-lp[1][i]], ends at s[i-1+lp[1][i]].
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/manacher.html
 */
template <typename Vec>
auto build_manachers(const Vec& s) {
    int N = s.size();
    array<vector<int>, 2> lp{vector<int>(N + 1), vector<int>(N)};
    for (int b : {0, 1}) {
        for (int i = 0, l = 0, r = -1; i < N; i++) {
            int k = (i > r) ? b : min(lp[b][l + r - i + !b], r - i + 1);
            while (0 <= i - k - !b && i + k < N && s[i - k - !b] == s[i + k]) {
                k++;
            }
            lp[b][i] = k--;
            if (i + k > r) {
                l = i - k - !b;
                r = i + k;
            }
        }
    }
    return lp;
}

#endif // STRINGS_HPP
