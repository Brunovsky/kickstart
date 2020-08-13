#include "../scc.hpp"

// *****

using vi = vector<int>;

void test() {
    strongly_connected_components scc(9);
    // vertex 0 is completely disconnected
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
    scc.compute();
    scc.condensate();
    printf("components #1: %d\n", scc.C);
    assert(scc.C == 5);
    for (int c = 0; c < scc.C; c++) {
        sort(begin(scc.cset[c]), end(scc.cset[c]));
        sort(begin(scc.cadjout[c]), end(scc.cadjout[c]));
        sort(begin(scc.cadjin[c]), end(scc.cadjin[c]));
    }
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

    ofstream file("datasets/scc.dot");
    file << to_dot(scc);
}

int main() {
    test();
    return 0;
}
