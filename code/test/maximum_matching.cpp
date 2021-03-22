#include "../maximum_matching.hpp"

#include "test_utils.hpp"

// *****

void unit_test_maximum_matching() {
    edges_t g;

    g = {{1, 1}, {1, 3}, {2, 4}, {2, 5}, {3, 2}, {3, 3}, {4, 3}, {4, 4}, {5, 3}};
    int mm0 = maximum_matching(6, 6, g).compute();
    int mm1 = hopcroft_karp(6, 6, g).compute();
    assert(mm0 == 5 && mm1 == 5);

    g = {{1, 1}, {1, 4}, {2, 3}, {2, 6}, {2, 7}, {3, 2}, {3, 4}, {3, 5}, {4, 2},
         {4, 7}, {5, 5}, {5, 6}, {5, 7}, {6, 3}, {6, 6}, {7, 6}, {7, 7}};
    mm0 = maximum_matching(8, 8, g).compute();
    mm1 = hopcroft_karp(8, 8, g).compute();
    assert(mm0 == 7 && mm1 == 7);

    print_ok("unit test maximum matching\n");
}

int main() {
    unit_test_maximum_matching();
    return 0;
}
