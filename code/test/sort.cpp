#include "../sort.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

bool verify(const vector<int>& idx, const vector<long>& dist) {
    int N = dist.size();
    if (idx.size() != N)
        return false;
    for (int i = 0; i + 1 < N; i++)
        if (dist[idx[i]] > dist[idx[i + 1]])
            return false;
    return true;
}

void std_sort(vector<int>& idx, vector<long>& dist) {
    int N = dist.size();
    idx.resize(N);
    iota(begin(idx), end(idx), 0);
    sort(begin(idx), end(idx), [&](int u, int v) { return dist[u] < dist[v]; });
}

void test(int T = 100) {
    START_ACC(inplace_lsb_radix);
    START_ACC(inplace_msb_radix);
    START_ACC(std);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "sort test");
        vector<long> dist = int_gen<long>(500'000, 100'000, 100'000'000);
        vector<int> idx[3];

        START(inplace_lsb_radix);
        lsb_radix_sort_idx(idx[0], dist);
        ADD_TIME(inplace_lsb_radix);

        START(inplace_msb_radix);
        msb_radix_sort_idx(idx[1], dist);
        ADD_TIME(inplace_msb_radix);

        START(std);
        std_sort(idx[2], dist);
        ADD_TIME(std);

        assert(verify(idx[0], dist));
        assert(verify(idx[1], dist));
        assert(verify(idx[2], dist));
    }

    PRINT_TIME(inplace_lsb_radix);
    PRINT_TIME(inplace_msb_radix);
    PRINT_TIME(std);
}

int main() {
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    test();
    return 0;
}
