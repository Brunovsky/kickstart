#include <cassert>
#include <iostream>
#include <memory>

using namespace std;

// *****

/**
 * STL-friendly AVL tree template for (multi)sets and (multi)maps.
 *   - Parametrized by type T and comparison functor.
 *   - Does not use allocators (for simplicity)
 *
 * Done:
 *   - AVL tree rotations and rebalancing for insertions and deletions
 *   - Single insert and erase
 *   - All iterators
 *   - Support comparison operators
 *
 * TODO:
 *   - Define set, multiset wrappers
 *   - Define map, multimap wrappers
 *   - Support emplace() and emplace_hint()
 *   - Support equal_range(), lower_bound() and upper_bound()
 *   - Support merge() and extract()
 *
 * Improvements:
 *   - Add small pool of dropped avl_nodes for reuse
 *   - Add constant-time minimum() and maximum() to avl_tree
 *   - Add order statistics or some other node update policy
 *   - Hide node balance in parent pointer (how?)
 *   - Support range insert and erase
 */

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
 *     new:    balance(y) = 0    balance(x) = 0   (height reduced)
 *   balance(x) == +2:
 *     heights: h, h-1, h+1
 *     new:    balance(y) = -1   balance(x) = 0   (height reduced)
 * }
 *
 * Note: the height delta (difference in height compared to the previous tree)
 * is -1 (i.e. the height diminished) iff rotations occurred and the new root
 * is 0-balanced.
 */

#define DEBUG_AVL_TREE 1

template <typename T>
struct avl_node {
    using node_t = avl_node<T>;

    node_t* parent = nullptr;
    node_t* link[2] = {};
    unique_ptr<T> data;
    int8_t balance = 0;

    avl_node() {}
    avl_node(node_t* parent, T data) : parent(parent), data(make_unique<T>(move(data))) {}

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
        return n->parent;
    }
    static const node_t* increment(const node_t* n) noexcept {
        if (n->link[1])
            return minimum(n->link[1]);
        while (n->parent && n == n->parent->link[1])
            n = n->parent;
        return n->parent;
    }
    static node_t* decrement(node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent && n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }
    static const node_t* decrement(const node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent && n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }
};

template <typename T>
struct avl_iterator {
    using node_t = avl_node<T>;
    node_t* y;

    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using self_type = avl_iterator<T>;
    using iterator_category = bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    avl_iterator() : y(nullptr) {}
    explicit avl_iterator(node_t* y) : y(y) {}

    T& operator*() const noexcept {
        return *y->data;
    }
    T* operator->() const noexcept {
        return y->data.get();
    }
    self_type& operator++() noexcept {
        assert(y);
        y = node_t::increment(y);
        return *this;
    }
    self_type operator++(int) noexcept {
        assert(y);
        self_type z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_type& operator--() noexcept {
        assert(y);
        y = node_t::decrement(y);
        return *this;
    }
    self_type operator--(int) noexcept {
        assert(y);
        self_type z = *this;
        y = node_t::decrement(y);
        return z;
    }
    friend bool operator==(const self_type& lhs, const self_type& rhs) noexcept {
        return lhs.y == rhs.y;
    }
    friend bool operator!=(const self_type& lhs, const self_type& rhs) noexcept {
        return lhs.y != rhs.y;
    }
};

template <typename T>
struct avl_const_iterator {
    using node_t = avl_node<T>;
    using iterator_t = avl_iterator<T>;
    const node_t* y;

    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using self_type = avl_const_iterator<T>;
    using iterator_category = bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    avl_const_iterator() : y(nullptr) {}
    explicit avl_const_iterator(const node_t* y) : y(y) {}
    explicit avl_const_iterator(const iterator_t& it) : y(it->y) {}

    const T& operator*() const noexcept {
        return *y->data;
    }
    const T* operator->() const noexcept {
        return y->data.get();
    }
    self_type& operator++() noexcept {
        assert(y);
        y = node_t::increment(y);
        return *this;
    }
    self_type operator++(int) noexcept {
        assert(y);
        self_type z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_type& operator--() noexcept {
        assert(y);
        y = node_t::decrement(y);
        return *this;
    }
    self_type operator--(int) noexcept {
        assert(y);
        self_type z = *this;
        y = node_t::decrement(y);
        return z;
    }
    friend bool operator==(const self_type& lhs, const self_type& rhs) noexcept {
        return lhs.y == rhs.y;
    }
    friend bool operator!=(const self_type& lhs, const self_type& rhs) noexcept {
        return lhs.y != rhs.y;
    }
};

template <typename T, typename CmpFn = less<T>>
struct avl_tree {
    using iterator = avl_iterator<T>;
    using const_iterator = avl_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;

    using node_t = avl_node<T>;
    using tree_t = avl_tree<T, CmpFn>;

    // The real tree's root is head->link[0]. head is never nullptr.
    node_t* head;
    size_t node_count = 0;
    CmpFn cmp;

    inline bool do_compare(const T& lhs, const T& rhs) const {
        return cmp(lhs, rhs);
    }

    inline node_t* minimum() noexcept {
        return head->link[0] ? node_t::minimum(head->link[0]) : nullptr;
    }
    inline const node_t* minimum() const noexcept {
        return head->link[0] ? node_t::minimum(head->link[0]) : nullptr;
    }
    inline node_t* maximum() noexcept {
        return head->link[0] ? node_t::maximum(head->link[0]) : nullptr;
    }
    inline const node_t* maximum() const noexcept {
        return head->link[0] ? node_t::maximum(head->link[0]) : nullptr;
    }

    inline iterator begin() noexcept {
        return iterator(minimum());
    }
    inline iterator end() noexcept {
        return iterator(head);
    }
    inline const_iterator begin() const noexcept {
        return const_iterator(minimum());
    }
    inline const_iterator end() const noexcept {
        return const_iterator(head);
    }
    inline const_iterator cbegin() const noexcept {
        return const_iterator(minimum());
    }
    inline const_iterator cend() const noexcept {
        return const_iterator(head);
    }
    inline reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    inline reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    inline const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    inline const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    inline const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    inline const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    friend bool operator==(const tree_t& lhs, const tree_t& rhs) noexcept {
        return lhs.size() == rhs.size() && equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    friend bool operator!=(const tree_t& lhs, const tree_t& rhs) noexcept {
        return !(lhs == rhs);
    }
    friend bool operator<(const tree_t& lhs, const tree_t& rhs) noexcept {
        return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
    friend bool operator>(const tree_t& lhs, const tree_t& rhs) noexcept {
        return rhs < lhs;
    }
    friend bool operator<=(const tree_t& lhs, const tree_t& rhs) noexcept {
        return !(rhs < lhs);
    }
    friend bool operator>=(const tree_t& lhs, const tree_t& rhs) noexcept {
        return !(lhs < rhs);
    }

    /**
     * AVL functions
     *
     * We handle rotations and rebalancing professionally, namely:
     *   - roots which are unbalanced 'know' how to rebalance themselves
     *   - rotation functions maintain invariants themselves
     *   - rotation functions should compose, so that e.g. a left-right rotation can
     *     be literally coded as a right rotation followed by a left rotation and
     *     the result will be exactly what is expected, with invariants maintained
     */

    inline void drop_node(node_t* node) {
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
     * Recalibrate the tree rooted at y that has become unbalanced, deducing
     * the necessary rotations. Does nothing if y is already balanced.
     * Returns the new root after calibration.
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

    void rebalance_after_erase(node_t* y) {
        if (y == head) {
            return;
        }
        y = rebalance(y);
        // recalibrate the tree until we reach a node that is 0-balanced
        while (y->parent != head && y->balance == 0) {
            bool is_right = y == y->parent->link[1];
            y->parent->balance += is_right ? -1 : 1;
            y = rebalance(y->parent);
        }
    }

    //             p  --> first ancestor of y that was not 0-balanced
    //            / \.
    //          [a] [b]
    //          /     \.
    //        ...     ...
    //        /         \.
    //      [d]         [c]
    //      /           /
    //    [e]          y
    //     ^--> p became 0-balanced here, otherwise it needs a rebalance
    void rebalance_after_insert(node_t* y) {
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
            rebalance_after_erase(parent);
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
        rebalance_after_erase(x);
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
    void erase_node_minimum(node_t* y) {
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
        rebalance_after_erase(w);
    }

    void erase_node_and_rebalance(node_t* y) {
        if (!y->link[1]) {
            erase_node_pull_left(y);
        } else if (!y->link[1]->link[0]) {
            erase_node_pull_right(y);
        } else {
            erase_node_minimum(y);
        }
    }

    void insert_node(node_t* parent, node_t* y, bool is_right) {
        adopt_node(parent, y, is_right);
        rebalance_after_insert(y);
        node_count++;
    }

    void erase_node(node_t* y) {
        erase_node_and_rebalance(y);
        drop_node(y);
        node_count--;
    }

  public:
    std::pair<iterator, bool> insert_unique(const T& data) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = do_compare(data, *y->data);
            if (!lesser && !do_compare(*y->data, data)) {
                return {end(), false};
            }
            parent = y;
            y = y->link[!lesser];
        }
        y = new node_t(parent, data);
        insert_node(parent, y, !lesser);
        return {iterator(y), true};
    }

    iterator insert_multi(const T& data) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = do_compare(data, *y->data);
            parent = y;
            y = y->link[!lesser];
        }
        y = new node_t(parent, data);
        insert_node(parent, y, !lesser);
        return iterator(y);
    }

    bool erase_unique(const T& data) {
        node_t* y = find(data);
        if (y) {
            erase_node(y);
            return true;
        }
        return false;
    }

    avl_tree(const CmpFn& cmp = CmpFn()) : head(new node_t()), cmp(cmp) {}

    ~avl_tree() {
        delete head;
    }

    void clear() {
        delete head;
        head = new node_t();
        node_count = 0;
    }

    size_t size() const noexcept {
        return node_count;
    }

    bool empty() const noexcept {
        return node_count == 0;
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
