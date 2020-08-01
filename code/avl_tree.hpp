#include <cassert>
#include <iostream>
#include <memory>

using namespace std;

// *****

// jesus christ there's like no good sources for this shit...
// boost/intrusive/avltree -> unparseable rotations
// wikipedia -> parseable but horrid
// geeksforgeeks -> horrid C code as usual
// https://www.javatpoint.com/deletion-in-avl-tree -> nice pictures

/**
 * Left rotation notes
 *
 *       y                     x
 *      / \                   / \
 *    [a]  x        ->       y  [c]
 *        / \               / \
 *      [b] [c]           [a] [b]
 *
 * height(a) = h
 * height(b) = {h-1, h, h+1}
 * height(c) = {h-1, h, h+1}
 *
 * assert(balance(y) >= +1 and balance(y) >= balance(x))
 *
 * balance(y) == +1 {
 *   balance(x) == -1:
 *     heights: h, h, h-1
 *     new:    balance(y) = 0    balance(x) = -2  (!)
 *   balance(x) == 0:
 *     heights: h, h, h
 *     new:    balance(y) = 0    balance(x) = -1
 *   balance(x) == +1:
 *     heights: h, h-1, h
 *     new:    balance(y) = -1   balance(x) = -1
 * }
 *
 * balance(y) == +2 {
 *   balance(x) == -1:
 *     heights: h, h+1, h
 *     new:    balance(y) = +1   balance(x) = -2  (!)
 *   balance(x) == 0:
 *     heights: h, h+1, h+1
 *     new:    balance(y) = +1   balance(x) = -1
 *   balance(x) == +1:
 *     heights: h, h, h+1
 *     new:    balance(y) = 0    balance(x) = 0
 *   balance(x) == +2:
 *     heights: h, h-1, h+1
 *     new:    balance(y) = -1   balance(x) = 0
 * }
 */

#define DEBUG_AVL_TREE 1

template <typename T>
struct avl_node {
    using node_t = avl_node<T>;

    node_t* parent = nullptr;
    node_t* link[2] = {};
    unique_ptr<T> data;
    int8_t balance = 0;

    avl_node(node_t* parent, T data) : parent(parent), data(make_unique<T>(move(data))) {}

    avl_node() {}

    ~avl_node() {
        delete link[0];
        delete link[1];
    }

    static node_t* minimum(node_t* n) noexcept {
        while (n->link[0])
            n = n->link[0];
        return n;
    }

    static const node_t* minimum(const node_t* n) noexcept {
        while (n->link[0])
            n = n->link[0];
        return n;
    }

    static node_t* maximum(node_t* n) noexcept {
        while (n->link[1])
            n = n->link[1];
        return n;
    }

    static const node_t* maximum(const node_t* n) noexcept {
        while (n->link[1])
            n = n->link[1];
        return n;
    }

    static node_t* increment(node_t* n) noexcept {
        if (n->link[1])
            return minimum(n->link[1]);
        while (n->parent && n == n->parent->link[1])
            n = n->parent;
        return n->parent ? minimum(n->parent) : nullptr;
    }

    static const node_t* increment(const node_t* n) noexcept {
        if (n->link[1])
            return minimum(n->link[1]);
        while (n->parent && n == n->parent->link[1])
            n = n->parent;
        return n->parent ? minimum(n->parent) : nullptr;
    }

    static node_t* decrement(node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent && n == n->parent->link[0])
            n = n->parent;
        return n->parent ? maximum(n->parent) : nullptr;
    }

    static const node_t* decrement(const node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent && n == n->parent->link[0])
            n = n->parent;
        return n->parent ? maximum(n->parent) : nullptr;
    }
};

template <typename T, typename CmpFn = less<T>, bool multiset = false, bool map = false>
class avl_tree {
    using node_t = avl_node<T>;

    // The real tree's root is head->link[0]. head is never nullptr.
    node_t* head;
    size_t node_count = 0;
    CmpFn cmp;

    inline bool do_compare(const T& lhs, const T& rhs) const {
        return cmp(lhs, rhs);
    }

    inline void delete_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        delete node;
    }

    inline void adopt_node(node_t* parent, node_t* child, bool is_right) {
        parent->link[is_right] = child;
        if (child)
            child->parent = parent;
    }

    /**
     *       y                     x
     *      / \                   / \
     *    [a]  x        ->       y  [c]
     *        / \               / \
     *      [b] [c]           [a] [b]
     * }
     */
    node_t* left_rotate(node_t* y) {
        node_t* x = y->link[1];
        assert(y->balance >= +1 && y->balance >= x->balance);
        bool is_right = y == y->parent->link[1];
        adopt_node(y->parent, x, is_right);
        adopt_node(y, x->link[0], 1);
        adopt_node(x, y, 0);
        int xb = x->balance;
        int y1 = y->balance == +1;
        int y2 = y->balance == +2;
        y->balance = -max(xb - y2, -y2);
        x->balance = min(xb - 1, -y1);
        return x;
    }

    /**
     *         y                  x
     *        / \                / \
     *       x  [c]     ->     [a]  y
     *      / \                    / \
     *    [a] [b]                [b] [c]
     */
    node_t* right_rotate(node_t* y) {
        node_t* x = y->link[0];
        assert(y->balance <= -1 && y->balance <= x->balance);
        bool is_right = y == y->parent->link[1];
        adopt_node(y->parent, x, is_right);
        adopt_node(y, x->link[1], 0);
        adopt_node(x, y, 1);
        int xb = x->balance;
        int y1 = y->balance == -1;
        int y2 = y->balance == -2;
        y->balance = -min(xb + y2, y2);
        x->balance = max(xb + 1, y1);
        return x;
    }

    /**
     * Recalibrates the tree rooted at y that has become unbalanced, deducing
     * the necessary rotations. Does nothing if y is already balanced.
     * Returns the new root after calibration.
     * Note: the height delta (difference in height compared to the previous tree)
     * is -1 (i.e. the height diminished) iff rotations occurred and the new root
     * is perfectly balanced.
     */
    node_t* rebalance(node_t* y) {
        assert(y && y != head);
        if (y->balance == -2) {
            if (y->link[0]->balance == +1) {
                left_rotate(y->link[0]);
            }
            return right_rotate(y);
        }
        if (y->balance == +2) {
            if (y->link[1]->balance == -1) {
                right_rotate(y->link[1]);
            }
            return left_rotate(y);
        }
        return y;
    }

    void rebalance_erase(node_t* y) {
        if (y == head) {
            return;
        }
        y = rebalance(y);
        while (y->parent != head && y->balance == 0) {
            bool is_right = y == y->parent->link[1];
            y->parent->balance += is_right ? -1 : 1;
            y = y->parent;
            y = rebalance(y);
        }
    }

    //             p  --> first ancestor of y that was non 0-balanced
    //            / \.
    //          [a] [b]
    //          /     \.
    //        ...     ...
    //        /         \.
    //      [d]         [c]
    //      /           /
    //    [e]          y
    //     ^--> p became 0-balanced here, otherwise it needs a rebalance
    void rebalance_insert(node_t* y) {
        node_t* parent = y->parent;

        // walk up the tree until we find an imperfectly balanced parent
        while (parent != head && parent->balance == 0) {
            bool is_right = y == parent->link[1];
            parent->balance = is_right ? +1 : -1;
            y = parent;
            parent = y->parent;
        }
        // found the imperfectly balanced parent, or root
        if (parent != head) {
            bool is_right = y == parent->link[1];
            parent->balance += is_right ? +1 : -1;
            rebalance(parent);
        }
    }

    //   parent       parent  <-- rebalance here
    //     |            |
    //     y           [x]
    //    /      ->
    //  [x]
    //                 balance(parent) := ±1
    void erase_node_pull_left(node_t* y) {
        node_t* x = y->link[0];
        node_t* parent = y->parent;
        bool y_is_right = y == parent->link[1];
        adopt_node(parent, x, y_is_right);
        if (parent != head) {
            parent->balance += y_is_right ? -1 : 1;
            rebalance_erase(parent);
        }
    }

    //     |            |
    //     y            x  <-- rebalance here
    //    / \    ->    / \.
    //  [a]  x       [a] [b]
    //        \.
    //        [b]
    //               balance(x) := balance(y) - 1
    void erase_node_pull_right(node_t* y) {
        node_t* x = y->link[1];
        node_t* parent = y->parent;
        bool y_is_right = y == parent->link[1];
        adopt_node(parent, x, y_is_right);
        adopt_node(x, y->link[0], 0);
        x->balance = y->balance - 1;
        rebalance_erase(x);
    }

    //        |                       |
    //        y                       x
    //       / \                     / \.
    //     [a]  x1                 [a]  x1
    //         / \                     / \.
    //       ... [b]      ->         ... [b]
    //       / \                     / \.
    //      w  [c]   rebalance -->  w  [c]
    //     / \          here       / \.
    //    x  [d]                 [e]  [d]
    //     \.
    //     [e]
    //                       balance(x) := balance(y)
    //                       balance(w) := balance(w) + 1
    void erase_node_search(node_t* y) {
        node_t* x = node_t::minimum(y->link[1]->link[0]);
        node_t* w = x->parent;
        node_t* parent = y->parent;
        bool y_is_right = y == parent->link[1];
        adopt_node(parent, x, y_is_right);
        adopt_node(w, x->link[1], 0);
        adopt_node(x, y->link[0], 0);
        adopt_node(x, y->link[1], 1);
        x->balance = y->balance;
        w->balance += 1;
        rebalance_erase(w);
    }

    void erase_node(node_t* y) {
        if (!y->link[1]) {
            erase_node_pull_left(y);
        } else if (!y->link[1]->link[0]) {
            erase_node_pull_right(y);
        } else {
            erase_node_search(y);
        }
    }

    pair<node_t*, bool> insert_search(const T& data) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = do_compare(data, *y->data);
            if (!multiset && !lesser && !do_compare(*y->data, data)) {
                return {y, false};
            }
            parent = y;
            y = y->link[!lesser];
        }
        y = parent->link[!lesser] = new node_t(parent, data);
        rebalance_insert(y);
        node_count++;
        return {y, true};
    }

    bool erase_search(const T& data) {
        node_t* y = head->link[0];
        bool lesser = true;
        while (y) {
            lesser = do_compare(data, *y->data);
            if (!lesser && !do_compare(*y->data, data)) {
                erase_node(y);
                delete_node(y);
                node_count--;
                return true;
            }
            y = y->link[!lesser];
        }
        return false;
    }

    node_t* find(const T& data) {
        node_t* y = head->link[0];
        while (y) {
            bool lesser = do_compare(data, *y->data);
            if (!lesser && !do_compare(*y->data, data)) {
                return y;
            }
            y = y->link[!lesser];
        }
        return nullptr;
    }

    node_t* lower_bound(const T& data) {
        node_t *y = head->link[0], parent = head;
        while (y) {
            bool lesser = do_compare(*y->data, data);
            parent = y;
            y = y->link[lesser];
        }
        return parent;
    }

    node_t* upper_bound(const T& data) {
        node_t *y = head->link[0], parent = head;
        while (y) {
            bool lesser = do_compare(data, *y->data);
            parent = y;
            y = y->link[!lesser];
        }
        return parent;
    }

public:
    avl_tree(const CmpFn& cmp = CmpFn()) : head(new node_t()), cmp(cmp) {}

    ~avl_tree() {
        delete head;
    }

    /**
     * Clear the set and all of its elements.
     * After this operation the set has size 0, as if it were default-constructed.
     */
    void clear() {
        delete head;
        head = new node_t();
        node_count = 0;
    }

    /**
     * Returns the number of elements in the set (counting repeated elements in
     * a multiset)
     */
    size_t size() const noexcept {
        return node_count;
    }

    /**
     * Returns true if the set has no elements.
     */
    bool empty() const noexcept {
        return node_count == 0;
    }

    /**
     * Returns true if the given element is in the set, and false otherwise.
     */
    bool contains(const T& data) const {
        node_t* y = head->link[0];
        while (y) {
            bool lesser = do_compare(data, *y->data);
            if (!lesser && !do_compare(*y->data, data)) {
                return true;
            }
            y = y->link[!lesser];
        }
        return false;
    }

    /**
     * Insert the given element into the set. Returns true if the element was
     * inserted, and false if the element already existed and this is not a multiset.
     */
    bool insert(const T& data) {
        return insert_search(data).second;
    }

    /**
     * Erase one instance of the given element in the set if it exists,
     * and otherise do nothing. Returns 1 if an element is removed and 0 otherwise.
     * For multisets, unlike std::multiset, this method will erase only one element.
     */
    size_t erase(const T& data) {
        return erase_search(data);
    }

#if DEBUG_AVL_TREE
private:
    void debug_print(const node_t* y, char S, int depth) const {
        string pad(2 * depth, ' ');
        if (!y) {
            printf("%s%c\n", pad.data(), S);
            return;
        }
        printf("%s%c %d (%d)\n", pad.data(), S, *y->data, y->balance);
        if (y->link[0] || y->link[1]) {
            debug_print(y->link[0], '<', depth + 1);
            debug_print(y->link[1], '>', depth + 1);
        }
    }

    int debug(const node_t* y, const node_t* parent, char side) const {
        if (!y) {
            return 0;
        }
        assert(y->parent == parent);
        if (side == '<') {
            assert(!do_compare(*parent->data, *y->data));
        } else if (side == '>') {
            assert(!do_compare(*y->data, *parent->data));
        }
        int left = debug(y->link[0], y, '<');
        int right = debug(y->link[1], y, '>');
        if (y->balance != right - left) {
            printf("BUG balance at %d(%+d)\n\n", *y->data, y->balance);
            debug_print();
        }
        assert(-1 <= y->balance && y->balance <= +1);
        assert(y->balance == right - left);
        return 1 + max(left, right);
    }

public:
    void debug_print() const {
        printf("====== nodes: %03lu ===========\n", node_count);
        debug_print(head->link[0], ' ', 0);
        printf("=============================\n");
    }

    void debug() const {
        assert(head && !head->link[1] && !head->data && head->balance == 0);
        debug(head->link[0], head, 'R');
    }
#endif
};

template <typename T, typename CmpFn = less<T>>
using avl_set = avl_tree<T, CmpFn, false>;

template <typename T, typename CmpFn = less<T>>
using avl_multiset = avl_tree<T, CmpFn, true>;
