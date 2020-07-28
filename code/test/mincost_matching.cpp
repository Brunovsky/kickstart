#include "../mincost_matching.hpp"

// *****

template <typename MM>
void test() {
    MM mm(2, 2);
    mm.add(0, 0, 7);
    mm.add(0, 1, 6);
    mm.add(1, 0, 5);
    mm.add(1, 1, 2);
    int min_cost = mm.compute();
    printf("min cost #1: %d\n", min_cost);
    assert(min_cost == 9);

    mm = MM(3, 3);
    mm.add(0, 0, 5);
    mm.add(0, 1, 4);
    mm.add(1, 0, 6);
    mm.add(1, 1, 3);
    mm.add(1, 2, 2);
    mm.add(2, 0, 1);
    mm.add(2, 2, 4);
    min_cost = mm.compute();
    printf("min cost #2: %d\n", min_cost);
    assert(min_cost == 7);

    mm = MM(3, 3);
    mm.add(0, 0, 108);
    mm.add(0, 1, 125);
    mm.add(0, 2, 150);
    mm.add(1, 0, 150);
    mm.add(1, 1, 135);
    mm.add(1, 2, 175);
    mm.add(2, 0, 122);
    mm.add(2, 1, 148);
    mm.add(2, 2, 250);
    min_cost = mm.compute();
    printf("min cost #3: %d\n", min_cost);
    assert(min_cost == 407);

    mm = MM(3, 4);
    mm.add(0, 0, 5);
    mm.add(0, 2, 3);
    mm.add(1, 0, 4);
    mm.add(1, 3, 1);
    mm.add(2, 1, 7);
    mm.add(2, 3, 2);
    min_cost = mm.compute();
    printf("min cost #4: %d\n", min_cost);
    assert(min_cost == 9);
}

int main() {
    test<mincost_matching>();
    return 0;
}
