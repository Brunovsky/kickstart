#include <fmt/format.h>

#include "../maximum_flow.hpp"

// *****

template <typename MF>
void test_g1() {
    MF mf(5);
    mf.add(1, 2, 1000);
    mf.add(1, 3, 1000);
    mf.add(2, 3, 1);
    mf.add(2, 4, 1000);
    mf.add(3, 4, 1000);
    int max_flow = mf.compute(1, 4);
    printf("max flow #1: %d\n", max_flow);
    assert(max_flow == 2000);
}

template <typename MF>
void test_g2() {
    MF mf(8);
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
    int max_flow = mf.compute(1, 7);
    printf("max flow #2: %d\n", max_flow);
    assert(max_flow == 5);
}

template <typename MF>
void test_g3() {
    MF mf(7);
    mf.add(1, 2, 15);
    mf.add(1, 4, 4);
    mf.add(2, 3, 12);
    mf.add(2, 6, 7);
    mf.add(3, 4, 3);
    mf.add(4, 5, 10);
    mf.add(5, 2, 5);
    mf.add(5, 6, 10);
    int max_flow = mf.compute(1, 6);
    printf("max flow #3: %d\n", max_flow);
    assert(max_flow == 14);
}

template <typename MF>
void test_g4() {
    MF mf(11);
    mf.add(0, 1, 20);
    mf.add(0, 2, 20);
    mf.add(0, 3, 20);
    mf.add(1, 4, 10);
    mf.add(2, 5, 10);
    mf.add(3, 6, 10);
    mf.add(1, 5, 10);
    mf.add(3, 5, 11);
    mf.add(4, 7, 10);
    mf.add(5, 8, 11);
    mf.add(6, 9, 10);
    mf.add(4, 8, 10);
    mf.add(8, 6, 10);
    mf.add(7, 10, 20);
    mf.add(8, 10, 20);
    mf.add(9, 10, 20);
    int max_flow = mf.compute(0, 10);
    printf("max flow #4: %d\n", max_flow);
}

template <typename MF>
void test(const string& which) {
    printf("%s:\n", which.data());
    test_g1<MF>();
    test_g2<MF>();
    test_g3<MF>();
    test_g4<MF>();
    printf("\n");
}

int main() {
    test<maximum_flow>("edmonds karp");
    test<maximum_flow_dinic>("dinic's blocking flows");
    test<maximum_flow_push_relabel>("push relabel");
    return 0;
}
