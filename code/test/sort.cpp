#include "test_utils.hpp"
#include "../algo/sort.hpp"

inline namespace detail {

template <typename T>
bool verify(const vector<int>& idx, const vector<T>& dist) {
    int N = dist.size();
    if (int(idx.size()) != N)
        return false;
    for (int i = 0; i + 1 < N; i++)
        if (dist[idx[i]] > dist[idx[i + 1]])
            return false;
    return true;
}

template <typename T>
void std_sort(vector<int>& idx, vector<T>& dist) {
    int N = dist.size();
    idx.resize(N);
    iota(begin(idx), end(idx), 0);
    sort(begin(idx), end(idx), [&](int u, int v) { return dist[u] < dist[v]; });
}

} // namespace detail

inline namespace speed_testing_radix_sort {

void speed_test_radix_sort_idx() {
    START_ACC(lsb);
    START_ACC(msb);
    START_ACC(std);

    LOOP_FOR_DURATION_TRACKED(3s, now) {
        print_time(now, 3s, 50ms, "speed test radix sort idx");

        vector<int> dist = int_gen<int>(500'000, 100'000, 100'000'000);
        vector<int> idx[3];

        START(lsb);
        lsb_radix_sort_idx<8>(idx[0], dist);
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

    PRINT_TIME_MS(lsb);
    PRINT_TIME_MS(msb);
    PRINT_TIME_MS(std);
}

void speed_test_radix_sort() {
    START_ACC(lsb);
    START_ACC(msb);
    START_ACC(std);

    LOOP_FOR_DURATION_TRACKED(3s, now) {
        print_time(now, 3s, 50ms, "speed test radix sort");

        auto v0 = int_gen<int>(500'000, 100'000, 100'000'000);
        auto v1 = v0, v2 = v0;

        START(lsb);
        lsb_radix_sort<8>(v0);
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

    PRINT_TIME_MS(lsb);
    PRINT_TIME_MS(msb);
    PRINT_TIME_MS(std);
}

} // namespace speed_testing_radix_sort

int main() {
    RUN_BLOCK(speed_test_radix_sort());
    RUN_BLOCK(speed_test_radix_sort_idx());
    return 0;
}
