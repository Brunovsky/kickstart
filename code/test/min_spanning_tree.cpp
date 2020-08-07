#include "../min_spanning_forest.hpp"

// *****

void test() {
    min_spanning_forest msf(7);
    msf.add(1, 2, 1);
    msf.add(1, 4, 4);
    msf.add(1, 5, 3);
    msf.add(2, 4, 4);
    msf.add(2, 5, 2);
    msf.add(3, 5, 4);
    msf.add(3, 6, 5);
    msf.add(4, 5, 4);
    msf.add(5, 6, 7);
    int weight = msf.compute();
    printf("mst weight #1: %d\n", weight);
    assert(weight == 16);
}

int main() {
    test();
    return 0;
}
