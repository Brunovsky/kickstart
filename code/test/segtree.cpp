#include "test_utils.hpp"
#include "../struct/segtree.hpp"
#include "../struct/dyn_segtree.hpp"

inline namespace unit_testing_segtree {

void unit_test_add_segtree() {
    using sum_segnode = samples_segtree::sum_segnode;
    using add_segupdate = samples_segtree::add_segupdate;

    vector<int> arr = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    int N = arr.size();

    segtree<sum_segnode, add_segupdate> tree(0, N, arr);

    assert(tree.query_range(0, 7).value == 42);
    tree.update_range(0, 3, 2);
    assert(tree.query_range(0, 7).value == 48);
    assert(tree.query_index(2).value == 9);
    tree.update_index(4, 10);
    assert(tree.query_range(0, 7).value == 58);
}

void unit_test_dyn_segtree() {
    using sum_segnode = samples_dyn_segtree::sum_segnode;
    using add_segupdate = samples_dyn_segtree::add_segupdate;

    dyn_segtree<sum_segnode> tree(10, 30);

    tree.update(14, add_segupdate(1));
    tree.update(13, add_segupdate(2));
    tree.update(27, add_segupdate(3));
    tree.update(21, add_segupdate(4));
    tree.update(24, add_segupdate(5));
    tree.update(18, add_segupdate(6));
    tree.update(21, add_segupdate(7));
    assert(tree.query(21, 22).value == 11);
    assert(tree.query(14, 18).value == 1);
    assert(tree.query(13, 19).value == 9);
    assert(tree.query(10, 30).value == 28);
}

} // namespace unit_testing_segtree

int main() {
    RUN_SHORT(unit_test_add_segtree());
    RUN_SHORT(unit_test_dyn_segtree());
    return 0;
}
