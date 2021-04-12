#include "../graphs/mincut.hpp"

#include "../formatting.hpp"
#include "test_utils.hpp"

// *****

// clang-format off

void unit_test_stoer_wagner() {
    edges_t g;
    vector<long> cost;
    long ans;
    cut_t cut;

    g = {{0, 1}, {0, 4}, {1, 2}, {1, 4}, {1, 5}, {2, 3},
         {2, 6}, {3, 6}, {3, 7}, {4, 5}, {5, 6}, {6, 7}};
    cost = {2, 3, 3, 2, 2, 4, 2, 2, 2, 3, 1, 3};
    tie(ans, cut) = stoer_wagner(8, g, cost);
    print("stoer_wagner 1 -- mincut: {}, ({})\n", ans, cut);
    assert(ans == 4);

    tie(ans, cut) = stoer_wagner(5, {
        {0, 130, 17, 12, 240},
        {130, 0, 200, 29, 24},
        {17, 200, 0, 150, 32},
        {12, 29, 150, 0, 210},
        {240, 24, 32, 210, 0},
    });
    print("stoer_wagner 2 -- mincut: {}, ({})\n", ans, cut);
    assert(ans == 382);

    print_ok("unit test stoer wagner");

}

int main() {
    unit_test_stoer_wagner();
    return 0;
}
