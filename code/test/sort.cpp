#include "../sort.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

// *****

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

void speed_test_radix_sort_idx(int T = 100) {
    START_ACC(lsb);
    START_ACC(msb);
    START_ACC(std);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "speed test radix sort idx");
        vector<long> dist = int_gen<long>(500'000, 100'000, 100'000'000);
        vector<int> idx[3];

        START(lsb);
        lsb_radix_sort_idx(idx[0], dist);
        ADD_TIME(lsb);

        START(msb);
        msb_radix_sort_idx(idx[1], dist);
        ADD_TIME(msb);

        START(std);
        std_sort(idx[2], dist);
        ADD_TIME(std);

        assert(verify(idx[0], dist));
        assert(verify(idx[1], dist));
        assert(verify(idx[2], dist));
    }

    clear_line(), print("speed test radix sort idx\n");
    PRINT_TIME(lsb);
    PRINT_TIME(msb);
    PRINT_TIME(std);
}

void speed_test_radix_sort(int T = 100) {
    START_ACC(lsb);
    START_ACC(msb);
    START_ACC(std);

    for (int i = 0; i < T; i++) {
        print_progress(i, T, "speed test radix sort");
        auto v0 = int_gen<long>(500'000, 100'000, 100'000'000);
        auto v1 = v0, v2 = v0;

        START(lsb);
        lsb_radix_sort(v0);
        ADD_TIME(lsb);

        START(msb);
        msb_radix_sort(v1);
        ADD_TIME(msb);

        START(std);
        sort(begin(v2), end(v2));
        ADD_TIME(std);

        assert(v0 == v2);
        assert(v1 == v2);
    }

    clear_line(), print("speed test radix sort\n");
    PRINT_TIME(lsb);
    PRINT_TIME(msb);
    PRINT_TIME(std);
}

int main() {
    speed_test_radix_sort();
    speed_test_radix_sort_idx();
    return 0;
}
