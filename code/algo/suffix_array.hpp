#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <bits/stdc++.h>

using namespace std;

/**
 * Compute the suffix array of a string/vec etc.
 * You should append to s a value smaller than any value in s and then pop the front
 * element of the returned SA. If not, the algorithm computes a sorting of rotations
 * instead, breaking ties arbitrarily.
 * To get the LCP as well do so before popping SA[0], and then pop LCP[0] as well.
 *
 * Complexity: O(N log N)
 * Reference: https://cp-algorithms.com/string/suffix-array.html
 */
template <typename Vec, typename T>
vector<int> build_cyclic_shifts(const Vec& s, int A, T first) {
    int N = s.size();
    if (N == 0)
        return {};

    int C = 0, S = 1; // #classes - 1, sorted width
    vector<int> cnt(max(A, N), 0), p(N, 0), c(N, 0), pn(N, 0), cn(N, 0);

    for (int i = 0; i < N; i++)
        cnt[s[i] - first]++;
    for (int a = 1; a < A; a++)
        cnt[a] += cnt[a - 1];
    for (int i = N - 1; i >= 0; i--)
        p[--cnt[s[i] - first]] = i;
    for (int i = 1; i < N; i++)
        c[p[i]] = C += s[p[i]] != s[p[i - 1]];

    while (S < N) {
        for (int i = 0; i < N; i++)
            pn[i] = p[i] - S, pn[i] += pn[i] < 0 ? N : 0;
        for (int c = 0; c <= C; c++)
            cnt[c] = 0;
        for (int i = 0; i < N; i++)
            cnt[c[pn[i]]]++;
        for (int a = 1; a <= C; a++)
            cnt[a] += cnt[a - 1];
        for (int i = N - 1; i >= 0; i--) // backwards for stable sort
            p[--cnt[c[pn[i]]]] = pn[i];

        if (2 * S >= N)
            break;

        cn[p[0]] = C = 0;
        for (int i = 1; i < N; i++) {
            int j = p[i] + S, k = p[i - 1] + S;
            j -= j >= N ? N : 0, k -= k >= N ? N : 0;
            cn[p[i]] = C += c[k] != c[j] || c[p[i]] != c[p[i - 1]];
        }
        swap(c, cn);
        S *= 2;
    }

    return p;
}

/**
 * Compute the LCP array for string/vec s and its suffix/cyclic shifts array.
 * LCP[i] = lcp(sa[i], sa[i+1]) for i=0,...,N.
 *
 * Complexity: O(N)
 */
template <typename Vec>
vector<int> build_lcp_array(const Vec& s, const vector<int>& sa) {
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
 * Compute the z function of string/vec s
 * z[i] := longest common prefix of s[0..] and s[i..]; z[i]=0.
 *
 * Complexity: O(N)
 * Reference: https://cp-algorithms.com/string/z-function.html
 */
template <typename Vec>
vector<int> build_z_function(const Vec& s) {
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

#endif // SUFFIX_ARRAY_HPP
