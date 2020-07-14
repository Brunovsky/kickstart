#include <bits/stdc++.h>

using namespace std;

/**
 * Static segment tree with indivisible segments in the leaves.
 * Segments are pre-populated and may be rotated to be aligned with the array
 * Ranges are inclusive
 */

// *****

namespace segment {

struct Node {
    int L, R;
    // TODO: data...
    int data;
};

int O, Q;
vector<Node> tree;

// populate [L,R) intervals from the leaves of the segment tree
void tree_lr() {
    for (int i = O - 1; i > 0; i--) {
        tree[i].L = tree[i << 1].L;
        tree[i].R = tree[i << 1 | 1].R;
        // TODO: initiate data in parent node
        tree[i].data = 0;
    }
}

// pushup an update in the whole tree
void pushup() {
    for (int i = O - 1; i > 0; i--) {
        // TODO: update node tree[i] with tree[i << 1] and tree[i << 1 | 1]
        tree[i].data += tree[i << 1].data + tree[i << 1 | 1].data;
    }
}

// pushup an update in tree[i]
void pushup(int i) {
    while ((i >>= 1) > 0) {
        // TODO: update node tree[i] with tree[i << 1] and tree[i << 1 | 1]
        tree[i].data += tree[i << 1].data + tree[i << 1 | 1].data;
    }
}

// pushdown aggregate data in the whole tree
void pushdown() {
    for (int i = 2; i < 2 * O; i++) {
        const auto &parent = tree[i >> 1];
        // TODO: pushdown parent data into tree[i]
        tree[i].data += parent.data;
    }
}

void pulldown(int i) {
    const auto &parent = tree[i >> 1];
    // TODO: pushdown parent data into tree[i]
    tree[i].data += parent.data;
    if (i < O) {
        pulldown(i << 1);
        pulldown(i << 1 | 1);
    }
}

// pushdown aggregate data in a part of the tree
void pushdown(int i) {
    if (i < O) {
        pulldown(i << 1);
        pulldown(i << 1 | 1);
    }
}

// query a range
auto query_range(int i, int L, int R) {
    // TODO: early exit test

    // overlap
    if (L <= tree[i].R && tree[i].L <= R) {
        // contain
        if (L <= tree[i].L && tree[i].R <= R) {
            // TODO: query tree[i], contained in [L,R]
            return tree[i].data * (R - L + 1);
        } else {
            assert(i < O);
            auto lhs = query_range(i << 1, L, R);
            auto rhs = query_range(i << 1 | 1, L, R);
            // TODO: aggregate results
            return lhs + rhs;
        }
    }

    // TODO: return empty query
    return 0;
}

// update a range
void update_range(int i, int L, int R, int data) {
    // TODO: early exit test

    // overlap
    if (L <= tree[i].R && tree[i].L <= R) {
        // contain
        if (L <= tree[i].L && tree[i].R <= R) {
            // TODO: update tree[i], contained in [L,R]
            tree[i].data += data;
        } else {
            assert(i < O);
            query_range(i << 1, L, R);
            query_range(i << 1 | 1, L, R);
        }
    }
}

// map array [0,O) to leaves [O,2O)
inline int to_leaf(int j) {
    return j < 2 * O - Q ? (j + Q) : (j + Q - O);
}

// map leaf [O,2O) to array [O,0)
inline int to_array(int i) {
    return i >= Q ? (i - Q) : (i + O - Q);
}

inline void align_leaves_to_tree() {
    rotate(tree.begin() + O, tree.begin() + (3 * O - Q), tree.end());
}

inline void align_leaves_to_array() {
    rotate(tree.begin() + O, tree.begin() + Q, tree.end());
}

// *****

struct Data {
    int L, R;
    int data;
    bool query;
};

void driver() {
    vector<int> endp;
    vector<Data> range_data;

    // .....

    sort(endp.begin(), endp.end());
    endp.erase(unique(endp.begin(), endp.end()), endp.end());
    O = endp.size() - 1;
    Q = 1;
    while (Q < O)
        Q <<= 1;

    tree.assign(2 * O, {});
    for (int i = 0; i < O; ++i) {
        tree[i + O].L = endp[i];
        tree[i + O].R = endp[i + 1] - 1;
    }

    align_leaves_to_tree();
    tree_lr();
    // ready

    // Case 1.
    // Multiple Inserts, 0 Queries
    // We care only about the values in the leaves
    for (auto entry : range_data) {
        update_range(1, entry.L, entry.R, entry.data);
    }
    pushdown();
    align_leaves_to_array();
    // now query tree[O]..tree[2O] which is ordered

    // Case 2.
    // Multiple Inserts, Queries afterwards
    for (auto entry : range_data) {
        update_range(1, entry.L, entry.R, entry.data);
    }
    pushdown();
    align_leaves_to_array();
    // now transform tree[O]..tree[2O] data into prefix sum form
    tree[O - 1].data = 0;
    for (int i = O; i < 2 * O; i++) {
        tree[i].data += tree[i - 1].data;
    }
    // now query [L,R] is tree[R] - tree[L-1].

    // Case 3.
    // Multiple Inserts, Multiple Queries, interleaved
    for (auto entry : range_data) {
        if (entry.query) {
            query_range(1, entry.L, entry.R);
        } else {
            update_range(1, entry.L, entry.R, entry.data);
        }
    }
    // done
}

} // namespace segment
