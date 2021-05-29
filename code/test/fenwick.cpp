#undef NDEBUG
#include "test_utils.hpp"
#include "../struct/fenwick.hpp"

inline namespace unit_testing_fenwick {

template <template <typename> typename Fenwick>
void unit_test_1d() {
    Fenwick<int> fw(100);

    fw.add(10, 10);
    fw.add(20, 20);
    fw.add(30, 30);
    fw.add(50, 100);
    fw.add(73, 80);
    fw.add(100, 300);
    assert(fw.sum(29) == 30);
    assert(fw.sum(30) == 60);
    assert(fw.sum(40) == 60);
    assert(fw.sum(70) == 160);
    assert(fw.sum(80) == 240);
    assert(fw.sum(100) == 540);
    assert(fw.lower_bound(30) == 20);
    assert(fw.lower_bound(31) == 30);
    assert(fw.lower_bound(159) == 50);
    assert(fw.lower_bound(540) == 100);
    assert(fw.lower_bound(541) == 101); // end is past N
}

template <template <typename> typename Fenwick2d>
void unit_test_2d() {
    Fenwick2d<int> fw(100, 100);

    fw.add(10, 10, 10);
    fw.add(20, 20, 20);
    fw.add(30, 50, 40);
    fw.add(50, 30, 40);
    fw.add(80, 80, 100);
    fw.add(5, 95, 60);
    fw.add(85, 5, 60);

    assert(fw.sum(40, 40) == 30);
    assert(fw.sum(10, 90) == 10);
    assert(fw.sum(40, 90) == 70);
    assert(fw.sum(70, 90) == 110);
    assert(fw.sum(80, 90) == 210);
    assert(fw.sum(80, 94) == 210);
    assert(fw.sum(80, 95) == 270);
}

} // namespace unit_testing_fenwick

int main() {
    RUN_SHORT(unit_test_1d<fenwick>());
    RUN_SHORT(unit_test_1d<sparse_fenwick>());
    RUN_SHORT(unit_test_2d<fenwick2d>());
    RUN_SHORT(unit_test_2d<sparse_fenwick2d>());
    return 0;
}
