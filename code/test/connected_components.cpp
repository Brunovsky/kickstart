#include "../connected_components.hpp"

// *****

void test() {
    strongly_connected_components scc(9);
    scc.add(1, 2);
    scc.add(2, 3);
    scc.add(3, 1);
    scc.add(4, 2);
    scc.add(4, 3);
    scc.add(4, 6);
    scc.add(5, 3);
    scc.add(5, 7);
    scc.add(6, 4);
    scc.add(6, 5);
    scc.add(7, 5);
    scc.add(8, 6);
    scc.add(8, 7);
    scc.add(8, 8);
    int num = scc.compute();
    printf("components #1: %d\n", num);
    assert(num == 5);
}

int main() {
    test();
    return 0;
}
