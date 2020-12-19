#include "../random.hpp"
#include "chrono.hpp"

bool verify(const vector<int>& idx, const vector<long>& dist) {
    int N = dist.size();
    if (idx.size() != N)
        return false;
    for (int i = 0; i + 1 < N; i++)
        if (dist[idx[i]] > dist[idx[i + 1]])
            return false;
    return true;
}

void radix_sort_idx(int* idx, int* cnt, int N, int B, int d, const vector<long>& dist) {
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

void radix_sort_buf(int* idx, int* buf, int* cnt, int N, int B, int d,
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
void msb_radix_sort_recurse(int* idx, int* buf, int N, int d, int maxd,
                            const vector<long>& dist) {
    constexpr int P = 1 << B;
    if (N <= 1 || d == maxd)
        return;
    if (N <= 20)
        return std::sort(idx, idx + N, [&](int u, int v) { return dist[u] < dist[v]; });

    int cnt[1 << B]{};
    memcpy(buf, idx, N * sizeof(int));
    radix_sort_buf(idx, buf, cnt, N, B, maxd - d - 1, dist);
    msb_radix_sort_recurse(idx, buf, cnt[0], d + 1, maxd, dist);
    for (int i = 1; i < P; i++) {
        int len = cnt[i] - cnt[i - 1];
        msb_radix_sort_recurse(idx + cnt[i - 1], buf, len, d + 1, maxd, dist);
    }
}

template <int B = 6>
void msb_radix_sort(vector<int>& idx, const vector<long>& dist) {
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
    radix_sort_idx(idx.data(), cnt, N, B, maxd - 1, dist);
    msb_radix_sort_recurse(idx.data(), buf, cnt[0], 1, maxd, dist);
    for (int i = 1; i < P; i++) {
        int len = cnt[i] - cnt[i - 1];
        msb_radix_sort_recurse(idx.data() + cnt[i - 1], buf, len, 1, maxd, dist);
    }
    delete[] buf;
}

template <int B = 6>
void lsb_radix_sort(vector<int>& idx, const vector<long>& dist) {
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
    radix_sort_idx(idx.data(), cnt, N, B, 0, dist);
    for (int d = 1; d < maxd; d++) {
        memset(cnt, 0, sizeof(cnt));
        swap(idx, buf);
        radix_sort_buf(idx.data(), buf.data(), cnt, N, B, d, dist);
    }
}

void std_sort(vector<int>& idx, vector<long>& dist) {
    int N = dist.size();
    idx.resize(N);
    iota(begin(idx), end(idx), 0);
    sort(begin(idx), end(idx), [&](int u, int v) { return dist[u] < dist[v]; });
}

void test(int R = 100) {
    START_ACC(inplace_lsb_radix);
    START_ACC(inplace_msb_radix);
    START_ACC(std);

    for (int i = 1; i <= R; i++) {
        print("\rsort test {}...", i);
        vector<long> dist = int_gen<long>(500'000, 100'000, 100'000'000);
        vector<int> idx[3];

        START(inplace_lsb_radix);
        lsb_radix_sort(idx[0], dist);
        ADD_TIME(inplace_lsb_radix);

        START(inplace_msb_radix);
        msb_radix_sort(idx[1], dist);
        ADD_TIME(inplace_msb_radix);

        START(std);
        std_sort(idx[2], dist);
        ADD_TIME(std);

        assert(verify(idx[0], dist));
        assert(verify(idx[1], dist));
        assert(verify(idx[2], dist));
    }

    PRINT_ACC(inplace_lsb_radix);
    PRINT_ACC(inplace_msb_radix);
    PRINT_ACC(std);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test();
    return 0;
}
