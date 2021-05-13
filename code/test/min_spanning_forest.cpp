#include "../graphs/min_spanning_forest.hpp"

#include "test_utils.hpp"

void unit_test_min_spanning_forest() {
    edges_t g;
    vector<long> weight;

    g = {{1, 2}, {1, 4}, {1, 5}, {2, 4}, {2, 5}, {3, 5}, {3, 6}, {4, 5}, {5, 6}};
    weight = {1, 4, 3, 4, 2, 4, 5, 4, 7};
    long w0 = min_spanning_forest_kruskal(7, g, weight);
    long w1 = min_spanning_forest_prim(7, g, weight);
    assert(w0 == 16 && w1 == 16);
}

int main() {
    RUN_SHORT(unit_test_min_spanning_forest());
    return 0;
}
