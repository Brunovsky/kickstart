#include "../struct/segment_tree.hpp"

#include "../formatting.hpp"
#include "test_utils.hpp"

// *****

void unit_test_range_segtree() {
    range_segtree tree({0, 4, 7, 12, 17, 25, 30, 40});
    tree.update(1, {7, 17}, 8);
    tree.update(1, {4, 30}, 3);
    tree.update(1, {12, 25}, 4);
    tree.update(1, {0, 7}, 2);
    assert(tree.query(1, {7, 12}) == 40 + 15);
    assert(tree.query(1, {17, 30}) == 39 + 32);
    assert(tree.query(1, {4, 17}) == 80 + 39 + 20 + 6);
    assert(tree.query(1, {0, 40}) == 224);
    assert(tree.to_array(tree.lower_bound(77)) == 2);
    assert(tree.to_array(tree.upper_bound(77)) == 2);
    assert(tree.to_array(tree.lower_bound(78)) == 2);
    assert(tree.to_array(tree.upper_bound(78)) == 3);
    assert(tree.to_array(tree.lower_bound(80)) == 3);
    assert(tree.to_array(tree.upper_bound(80)) == 3);
}

void unit_test_dyn_segtree() {
    dyn_segtree tree(10, 30);
    tree.add(14, 1);
    tree.add(13, 2);
    tree.add(27, 3);
    tree.add(21, 4);
    tree.add(24, 5);
    tree.add(18, 6);
    tree.add(21, 7);
    assert(tree.query(21, 22) == 11);
    assert(tree.query(14, 18) == 1);
    assert(tree.query(13, 19) == 9);
    assert(tree.query(10, 30) == 28);
}

int main() {
    RUN_SHORT(unit_test_range_segtree());
    RUN_SHORT(unit_test_dyn_segtree());
    return 0;
}
