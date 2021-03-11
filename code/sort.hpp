#ifndef SORT_HPP
#define SORT_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Radix sort for distance vectors
 * Useful for shortest path problems and the like
 * The output of the sort functions is a vector of indices into the dist vector
 * such that dist[idx[0]], dist[idx[1]]... is sorted.
 */

template <int B = 6>
void radix_sort_idx(int* idx, int* cnt, int N, int d, const vector<long>& dist) {
    const int s = B * d, P = 1 << B, mask = P - 1;
    for (int n = 0; n < N; n++) {
        cnt[(dist[n] >> s) & mask]++;
    }
    for (int sum = N, i = P - 1; i >= 0; i--) {
        sum -= cnt[i], cnt[i] = sum;
    }
    for (int n = 0; n < N; n++) {
        idx[cnt[(dist[n] >> s) & mask]++] = n;
    }
}

template <int B = 6>
void radix_sort_buf(int* idx, int* buf, int* cnt, int N, int d,
                    const vector<long>& dist) {
    const int s = B * d, P = 1 << B, mask = P - 1;
    for (int j = 0; j < N; j++) {
        cnt[(dist[buf[j]] >> s) & mask]++;
    }
    for (int sum = N, i = P - 1; i >= 0; i--) {
        sum -= cnt[i], cnt[i] = sum;
    }
    for (int j = 0; j < N; j++) {
        idx[cnt[(dist[buf[j]] >> s) & mask]++] = buf[j];
    }
}

template <int B = 6>
void msb_radix_sort_idx_recurse(int* idx, int* buf, int N, int d, int maxd,
                                const vector<long>& dist) {
    constexpr int P = 1 << B;
    if (N <= 1 || d == maxd)
        return;
    if (N <= 20)
        return std::sort(idx, idx + N, [&](int u, int v) { return dist[u] < dist[v]; });

    int cnt[P]{};
    memcpy(buf, idx, N * sizeof(int));
    radix_sort_buf<B>(idx, buf, cnt, N, maxd - d - 1, dist);
    msb_radix_sort_idx_recurse<B>(idx, buf, cnt[0], d + 1, maxd, dist);
    for (int i = 1; i < P; i++) {
        int len = cnt[i] - cnt[i - 1];
        msb_radix_sort_idx_recurse<B>(idx + cnt[i - 1], buf, len, d + 1, maxd, dist);
    }
}

template <int B = 6>
void msb_radix_sort_idx(vector<int>& idx, const vector<long>& dist) {
    constexpr int P = 1 << B;
    int N = dist.size(), maxd = 0;
    idx.resize(N);

    long max = *max_element(begin(dist), end(dist));
    while (max > 0)
        maxd++, max >>= B;
    if (maxd == 0)
        return iota(begin(idx), end(idx), 0);

    int cnt[P]{};
    int* buf = new int[N];
    radix_sort_idx<B>(idx.data(), cnt, N, maxd - 1, dist);
    msb_radix_sort_idx_recurse<B>(idx.data(), buf, cnt[0], 1, maxd, dist);
    for (int i = 1; i < P; i++) {
        int len = cnt[i] - cnt[i - 1];
        msb_radix_sort_idx_recurse<B>(idx.data() + cnt[i - 1], buf, len, 1, maxd, dist);
    }
    delete[] buf;
}

template <int B = 6>
void lsb_radix_sort_idx(vector<int>& idx, const vector<long>& dist) {
    constexpr int P = 1 << B;
    int N = dist.size(), maxd = 0;
    idx.resize(N);

    long max = *max_element(begin(dist), end(dist));
    while (max > 0)
        maxd++, max >>= B;
    if (maxd == 0)
        return iota(begin(idx), end(idx), 0);

    vector<int> buf(N);
    int cnt[P]{};
    radix_sort_idx<B>(idx.data(), cnt, N, 0, dist);
    for (int d = 1; d < maxd; d++) {
        memset(cnt, 0, sizeof(cnt));
        swap(idx, buf);
        radix_sort_buf<B>(idx.data(), buf.data(), cnt, N, d, dist);
    }
}

#endif // SORT_HPP
