#include "../maximum_flow.hpp"

// *****

void test() {
    maximum_flow mf(4);
    mf.add(1, 2, 1000);
    mf.add(1, 3, 1000);
    mf.add(2, 3, 1);
    mf.add(2, 4, 1000);
    mf.add(3, 4, 1000);
    mf.add_source(1);
    mf.add_sink(4);
    int max_flow = mf.compute();
    printf("max flow #1: %d\n", max_flow);
    assert(max_flow == 2000);

    mf = maximum_flow(7);
    mf.add(1, 2, 3);
    mf.add(1, 4, 3);
    mf.add(2, 3, 4);
    mf.add(3, 1, 3);
    mf.add(3, 4, 1);
    mf.add(3, 5, 2);
    mf.add(4, 5, 2);
    mf.add(4, 6, 6);
    mf.add(5, 2, 1);
    mf.add(5, 7, 1);
    mf.add(6, 7, 9);
    mf.add_source(1);
    mf.add_sink(7);
    max_flow = mf.compute();
    printf("max flow #2: %d\n", max_flow);
    assert(max_flow == 5);
}

int main() {
    test();
    return 0;
}
