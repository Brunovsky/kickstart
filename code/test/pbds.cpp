#include "test_utils.hpp"
#include "../struct/pbds.hpp"

inline namespace unit_testing_pbds_heap {

void unit_test_heap() {
    using heap_t = max_heap<pair<long, int>, greater<>>;
    using iter_t = heap_t::point_const_iterator;
    heap_t heap;
    vector<iter_t> iters(3);
    iters[0] = heap.push({10, 0});
    iters[1] = heap.push({6, 1});
    iters[2] = heap.push({4, 2});
    print("n: {}\n", heap.top().second);
    heap.modify(iters[0], {5, 0});
    print("n: {}\n", heap.top().second);
    heap.modify(iters[0], {3, 0});
    print("n: {}\n", heap.top().second);

    heap.pop();
}

} // namespace unit_testing_pbds_heap

int main() {
    RUN_SHORT(unit_test_heap());
    return 0;
}
