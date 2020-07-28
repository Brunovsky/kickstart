#include "../maximal_matching.hpp"

// *****

template <typename MM>
void test() {
    MM mm(6, 6);
    mm.add(1, 1), mm.add(1, 3);
    mm.add(2, 4), mm.add(2, 5);
    mm.add(3, 2), mm.add(3, 3);
    mm.add(4, 3), mm.add(4, 4);
    mm.add(5, 3);
    int res = mm.compute();
    assert(res == 5);

    mm = MM(8, 8);
    mm.add(1, 1), mm.add(1, 4);
    mm.add(2, 3), mm.add(2, 6), mm.add(2, 7);
    mm.add(3, 2), mm.add(3, 4), mm.add(3, 5);
    mm.add(4, 2), mm.add(4, 7);
    mm.add(5, 5), mm.add(5, 6), mm.add(5, 7);
    mm.add(6, 3), mm.add(6, 6);
    mm.add(7, 6), mm.add(7, 7);
    res = mm.compute();
    assert(res == 7);

    (void)res;
}

int main() {
    test<maximal_matching>();
    test<maximal_matching_big>();
    return 0;
}
