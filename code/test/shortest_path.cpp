#include "../shortest_path.hpp"

#include <fmt/format.h>

using fmt::print;

// *****

// https://en.wikipedia.org/wiki/Shortest_path_problem#/media/File:Shortest_path_with_direct_weights.svg
template <typename MM>
void add_g1(MM& mm) {
    mm = MM(6);
    mm.add(0, 1, 4);
    mm.add(0, 2, 2);
    mm.add(1, 2, 5);
    mm.add(1, 3, 10);
    mm.add(2, 4, 3);
    mm.add(3, 5, 11);
    mm.add(4, 3, 4);
}

template <typename MM>
void add_g2(MM& mm) {
    mm = MM(7);
    mm.add(1, 2, 7);
    mm.add(1, 3, 9);
    mm.add(1, 6, 14);
    mm.add(2, 3, 10);
    mm.add(2, 4, 15);
    mm.add(3, 4, 11);
    mm.add(3, 6, 2);
    mm.add(4, 5, 6);
    mm.add(5, 6, 9);
    mm.add(6, 5, 9); // double
}

void test() {
    dijkstra mm(6);
    add_g1(mm);
    mm.compute(0);
    assert(mm.dist[3] == 9);
    assert(mm.dist[5] == 20);
    assert(mm.path(5) == vector<int>({0, 2, 4, 3, 5}));

    add_g2(mm);
    mm.compute(1);
    assert(mm.dist[0] == LONG_MAX / 2);
    assert(mm.dist[5] == 20);
    assert(mm.dist[6] == 11);
    assert(mm.path(5) == vector<int>({1, 3, 6, 5}));
}

int main() {
    test();
    return 0;
}
