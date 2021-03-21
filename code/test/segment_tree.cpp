#include "../segment_tree.hpp"

#include "../debug_print.hpp"
#include "test_utils.hpp"

// Example usage segment tree

struct Data {
    int L, R;
    int data;
    bool query;
};

void driver() {
    vector<int> endp;
    segment_tree segtree;
    // .....

    sort(begin(endp), end(endp));
    endp.erase(unique(begin(endp), end(endp)), end(endp));

    segtree.setup(endp);
    // ready
}

void driver_usage() {
    vector<int> endp;
    vector<Data> range_data;
    segment_tree segtree;

    // Case 1.
    // Multiple Inserts, 0 Queries
    // We care only about the values in the leaves
    for (auto entry : range_data) {
        segtree.update_range(1, {entry.L, entry.R}, entry.data);
    }
    segtree.pushdown_all();
    segtree.align_leaves_to_array();
    // now query tree[O]..tree[2O] which is ordered

    // Case 2.
    // Multiple Inserts, Queries afterwards, prefix sum form exists
    for (auto entry : range_data) {
        segtree.update_range(1, {entry.L, entry.R}, entry.data);
    }
    segtree.pushdown_all();
    segtree.align_leaves_to_array();
    // now transform tree[O]..tree[2O] data into prefix sum form
    int O = segtree.O;
    segtree.tree[O - 1].data = 0;
    for (int i = O; i < 2 * O; i++) {
        segtree.tree[i].data += segtree.tree[i - 1].data;
        // now query [L,R] is tree[R] - tree[L-1].
    }

    // Case 3.
    // Multiple Inserts, Multiple Queries, interleaved or afterwards
    for (auto entry : range_data) {
        if (entry.query) {
            segtree.query_range(1, {entry.L, entry.R});
        } else {
            segtree.update_range(1, {entry.L, entry.R}, entry.data);
        }
    }
    // done
}

// *****

void unit_test_segment_tree() {
    segment_tree tree;
    tree.setup({0, 4, 7, 12, 17, 25, 30, 40});
    tree.update_range(1, {7, 16}, 8);
    tree.update_range(1, {4, 29}, 3);
    tree.update_range(1, {12, 24}, 4);
    tree.update_range(1, {0, 6}, 2);
    assert(tree.query_range(1, {7, 11}) == 40 + 15);
    assert(tree.query_range(1, {17, 29}) == 39 + 32);
    assert(tree.query_range(1, {4, 16}) == 80 + 39 + 20 + 6);
    print_ok("unit test segment tree");
}

int main() {
    unit_test_segment_tree();
    return 0;
}
