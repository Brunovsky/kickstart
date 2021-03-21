#include "../scc.hpp"

#include "test_utils.hpp"

// *****

void unit_test_scc() {
    // vertex 0 is completely disconnected
    edges_t g = {{1, 2}, {2, 3}, {3, 1}, {4, 2}, {4, 3}, {4, 6}, {5, 3},
                 {5, 7}, {6, 4}, {6, 5}, {7, 5}, {8, 6}, {8, 7}, {8, 8}};
    strongly_connected_components scc(9, g);

    scc.compute();
    scc.condensate();
    printf("components #1: %d\n", scc.C);

    assert(scc.C == 5); // num components
    for (int c = 0; c < scc.C; c++) {
        sort(begin(scc.cset[c]), end(scc.cset[c]));
        sort(begin(scc.cadjout[c]), end(scc.cadjout[c]));
        sort(begin(scc.cadjin[c]), end(scc.cadjin[c]));
    }

    using vi = vector<int>;

    assert(scc.cset[0] == vi({0}));
    assert(scc.cset[1] == vi({1, 2, 3}));
    assert(scc.cset[2] == vi({5, 7}));
    assert(scc.cset[3] == vi({4, 6}));
    assert(scc.cset[4] == vi({8}));

    assert(scc.cadjout[0] == vi() && scc.cadjin[0] == vi());
    assert(scc.cadjout[1] == vi() && scc.cadjin[1] == vi({2, 3}));
    assert(scc.cadjout[2] == vi({1}) && scc.cadjin[2] == vi({3, 4}));
    assert(scc.cadjout[3] == vi({1, 2}) && scc.cadjin[3] == vi({4}));
    assert(scc.cadjout[4] == vi({2, 3}) && scc.cadjin[4] == vi());

    print_ok("unit test strongly connected components");
}

int main() {
    unit_test_scc();
    return 0;
}
