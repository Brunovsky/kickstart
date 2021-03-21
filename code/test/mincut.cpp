#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../mincut.hpp"

#include "../debug_print.hpp"
#include "../graph.hpp"
#include "test_utils.hpp"

// *****

void unit_test_stoer_wagner() {
    edges_t g;
    costs_t cost;

    g = {{0, 1}, {0, 4}, {1, 2}, {1, 4}, {1, 5}, {2, 3},
         {2, 6}, {3, 6}, {3, 7}, {4, 5}, {5, 6}, {6, 7}};
    cost = {2, 3, 3, 2, 2, 4, 2, 2, 2, 3, 1, 3};
    stoer_wagner sw(8, g, cost);
    auto sol = sw.minimum_cut();
    print("stoer_wagner 1 mincut: {}\n", sol);
    print_ok("unit test stoer wagner");
}

int main() {
    unit_test_stoer_wagner();
    return 0;
}
