#include "../struct/lca.hpp"

#include "test_utils.hpp"

// *****

// https://www.geeksforgeeks.org/depth-n-ary-tree/
void setup() {
    children[1] = {2, 3, 4, 5};
    children[2] = {6, 7};
    children[3] = {8, 9, 10};
    children[4] = {};
    children[5] = {11, 12, 13};
    children[6] = {};
    children[7] = {14};
    children[8] = {};
    children[9] = {};
    children[10] = {15, 16};
    children[11] = {};
    children[12] = {};
    children[13] = {17, 18, 19};
    children[14] = {};
    children[15] = {};
    children[16] = {};
    children[17] = {};
    children[18] = {};
    children[19] = {};
}

void unit_test_lca() {
    lca_tree<20, 5> lca;
    lca.init(1);
    assert(lca.lca(11, 19) == 5);
    assert(lca.lca(9, 15) == 3);
    assert(lca.lca(14, 15) == 1);
    assert(lca.lca(11, 13) == 5);
    assert(lca.depth[8] == 2);
    assert(lca.depth[16] == 3);
    assert(lca.dist(7, 17) == 5);
    assert(lca.dist(6, 8) == 4);
    assert(lca.dist(3, 3) == 0);
    assert(lca.dist(3, 15) == 2);
    print_ok("unit test lca");
}

int main() {
    setup();
    unit_test_lca();
    return 0;
}
