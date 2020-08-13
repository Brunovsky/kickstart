#include "../maximum_flow.hpp"

// *****

template <typename MF>
void add_g1(MF& mf) {
    mf = MF(5);
    mf.add(1, 2, 1000);
    mf.add(1, 3, 1000);
    mf.add(2, 3, 1);
    mf.add(2, 4, 1000);
    mf.add(3, 4, 1000);
    // 1 4
}

template <typename MF>
void add_g2(MF& mf) {
    mf = MF(8);
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
    // 1 7
}

template <typename MF>
void add_g3(MF& mf) {
    mf = MF(7);
    mf.add(1, 2, 15);
    mf.add(1, 4, 4);
    mf.add(2, 3, 12);
    mf.add(2, 6, 7);
    mf.add(3, 4, 3);
    mf.add(4, 5, 10);
    mf.add(5, 2, 5);
    mf.add(5, 6, 10);
    // 1 6
}

template <typename MF>
void test() {
    MF mf(0);

    add_g1(mf);
    int max_flow = mf.compute(1, 4);
    printf("max flow #1: %d\n", max_flow);
    assert(max_flow == 2000);

    add_g2(mf);
    max_flow = mf.compute(1, 7);
    printf("max flow #2: %d\n", max_flow);
    assert(max_flow == 5);

    add_g3(mf);
    max_flow = mf.compute(1, 6);
    printf("max flow #3: %d\n", max_flow);
    assert(max_flow == 14);
}

int main() {
    test<maximum_flow>();
    test<maximum_flow_dinic>();
    test<maximum_flow_push_relabel>();
    return 0;
}
