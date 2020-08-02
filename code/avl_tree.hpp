#include <cassert>
#include <iostream>
#include <limits>
#include <memory>

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
 *   - Support equal_range(), lower_bound() and upper_bound()
 *   - Support emplace(), emplace_hint(), and insert_hint()
 *   - Copying and moving
 *
 * TODO:
 *   - Define set, multiset wrappers
 *   - Define map, multimap wrappers
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
    std::unique_ptr<T> data;
    int8_t balance = 0;

    avl_node() {}
    avl_node(T data) : data(std::make_unique<T>(std::move(data))) {}
    template <typename... Args>
    avl_node(Args&&... args) : data(std::make_unique<T>(std::forward<Args>(args)...)) {}

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
    using iterator_category = std::bidirectional_iterator_tag;
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
        y = node_t::increment(y);
        return *this;
    }
    self_type operator++(int) noexcept {
        self_type z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_type& operator--() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_type operator--(int) noexcept {
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
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    avl_const_iterator() : y(nullptr) {}
    explicit avl_const_iterator(const node_t* y) : y(y) {}
    avl_const_iterator(const iterator_t& it) : y(it.y) {}

    const T& operator*() const noexcept {
        return *y->data;
    }
    const T* operator->() const noexcept {
        return y->data.get();
    }
    self_type& operator++() noexcept {
        y = node_t::increment(y);
        return *this;
    }
    self_type operator++(int) noexcept {
        self_type z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_type& operator--() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_type operator--(int) noexcept {
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

template <typename T, typename CmpFn = std::less<T>>
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
    size_t node_count;
    CmpFn cmp;

    avl_tree(const CmpFn& cmp = CmpFn()) noexcept
        : head(new node_t()), node_count(0), cmp(cmp) {}

    // Move constructor
    avl_tree(avl_tree&& other) noexcept
        : head(new node_t()), node_count(other.node_count), cmp(std::move(other.cmp)) {
        adopt_node(head, other.head->link[0], 0);
        other.head->link[0] = nullptr; // note: we leave other in a semi-valid state
        other.node_count = 0;
    }

    // Copy constructor
    avl_tree(const avl_tree& other) noexcept
        : head(new node_t()), node_count(other.node_count), cmp(other.cmp) {
        adopt_node(head, deep_clone_node(other.head->link[0]), 0);
    }

    // Move assignment
    avl_tree& operator=(avl_tree&& other) noexcept {
        delete head->link[0];
        adopt_node(head, other.head->link[0], 0);
        node_count = other.node_count;
        cmp = std::move(other.cmp);
        other.head->link[0] = nullptr;
        other.node_count = 0;
        return *this;
    }

    // Copy assignment
    avl_tree& operator=(const avl_tree& other) noexcept {
        delete head->link[0];
        adopt_node(head, deep_clone_node(other.head->link[0]), 0);
        node_count = other.node_count;
        cmp = other.cmp;
        return *this;
    }

    ~avl_tree() noexcept {
        delete head;
    }

    void clear() noexcept {
        delete head->link[0];
        head->link[0] = nullptr;
        node_count = 0;
    }
    size_t size() const noexcept {
        return node_count;
    }
    bool empty() const noexcept {
        return node_count == 0;
    }
    size_t max_size() const noexcept {
        return std::numeric_limits<size_t>::max();
    }

    inline bool do_compare(const T& lhs, const T& rhs) const {
        return cmp(lhs, rhs);
    }

    inline node_t* minimum() noexcept {
        return head->link[0] ? node_t::minimum(head->link[0]) : head;
    }
    inline const node_t* minimum() const noexcept {
        return head->link[0] ? node_t::minimum(head->link[0]) : head;
    }
    inline node_t* maximum() noexcept {
        return head->link[0] ? node_t::maximum(head->link[0]) : head;
    }
    inline const node_t* maximum() const noexcept {
        return head->link[0] ? node_t::maximum(head->link[0]) : head;
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
        return lhs.size() == rhs.size() &&
               std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    friend bool operator!=(const tree_t& lhs, const tree_t& rhs) noexcept {
        return !(lhs == rhs);
    }
    friend bool operator<(const tree_t& lhs, const tree_t& rhs) noexcept {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                            rhs.end());
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

    template <typename K = T>
    node_t* find_node(const K& data) {
        node_t* x = head->link[0];
        while (x) {
            bool lesser = do_compare(data, *x->data);
            if (!lesser && !do_compare(*x->data, data))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = T>
    const node_t* find_node(const K& data) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = do_compare(data, *x->data);
            if (!lesser && !do_compare(*x->data, data))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = T>
    iterator find(const K& data) {
        return iterator(find_node(data));
    }
    template <typename K = T>
    const_iterator find(const K& data) const {
        return const_iterator(find_node(data));
    }
    template <typename K = T>
    iterator lower_bound(const K& data) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (!do_compare(*x->data, data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return iterator(y);
    }
    template <typename K = T>
    const_iterator lower_bound(const K& data) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (!do_compare(*x->data, data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return const_iterator(y);
    }
    template <typename K = T>
    iterator upper_bound(const K& data) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (do_compare(data, *x->data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return iterator(y);
    }
    template <typename K = T>
    const_iterator upper_bound(const K& data) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (do_compare(data, *x->data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return const_iterator(y);
    }
    template <typename K = T>
    std::pair<iterator, iterator> equal_range(const K& data) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (do_compare(*x->data, data))
                x = x->link[1];
            else if (do_compare(data, *x->data))
                y = x, x = x->link[0];
            else {
                node_t* xu = x->link[1];
                node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!do_compare(*x->data, data))
                        y = x, x = x->link[0];
                    else
                        x = x->link[1];
                }
                // upper bound [xu, yu]
                while (xu) {
                    if (do_compare(data, *xu->data))
                        yu = xu, xu = xu->link[0];
                    else
                        xu = xu->link[1];
                }
                return {iterator(y), iterator(yu)};
            }
        }
        return {iterator(y), iterator(y)};
    }
    template <typename K = T>
    std::pair<const_iterator, const_iterator> equal_range(const K& data) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (do_compare(*x->data, data))
                x = x->link[1];
            else if (do_compare(data, *x->data))
                y = x, x = x->link[0];
            else {
                const node_t* xu = x->link[1];
                const node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!do_compare(*x->data, data))
                        y = x, x = x->link[0];
                    else
                        x = x->link[1];
                }
                // upper bound [xu, yu]
                while (xu) {
                    if (do_compare(data, *xu->data))
                        yu = xu, xu = xu->link[0];
                    else
                        xu = xu->link[1];
                }
                return {const_iterator(y), const_iterator(yu)};
            }
        }
        return {const_iterator(y), const_iterator(y)};
    }
    template <typename K = T>
    bool contains(const K& data) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = do_compare(data, *x->data);
            if (!lesser && !do_compare(*x->data, data))
                return true;
            x = x->link[!lesser];
        }
        return false;
    }
    template <typename K = T>
    size_t count(const K& data) const {
        auto range = equal_range(data);
        return std::distance(range.first, range.second);
    }

    static inline void drop_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        delete node;
    }

    static inline void adopt_node(node_t* parent, node_t* child, bool is_right) {
        parent->link[is_right] = child;
        if (child)
            child->parent = parent;
    }

    static node_t* deep_clone_node(const node_t* node) {
        if (!node)
            return nullptr;
        node_t* clone = new node_t(*node->data);
        clone->balance = node->balance;
        adopt_node(clone, deep_clone_node(node->link[0]), 0);
        adopt_node(clone, deep_clone_node(node->link[1]), 1);
        return clone;
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

    /**
     *       y                     x
     *      / \                   / \
     *    [a]  x        ->       y  [c]
     *        / \               / \
     *      [b] [c]           [a] [b]
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
        y->balance = -std::max(xb - y2, -y2);
        x->balance = std::min(xb - 1, -y1);
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
        y->balance = -std::min(xb + y2, y2);
        x->balance = std::max(xb + 1, y1);
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
        // recalibrate the tree until we reach a node that is not 0-balanced
        while (y->parent != head && y->balance == 0) {
            bool is_right = y == y->parent->link[1];
            y->parent->balance += is_right ? -1 : 1;
            y = rebalance(y->parent);
        }
    }
    /**
     *             p  --> first ancestor of y that was not 0-balanced
     *            / \.
     *          [a] [b]
     *          /     \.
     *        ...     ...
     *        /         \.
     *      [d]         [c]
     *      /           /
     *    [e]          y
     *     ^--> p became 0-balanced here, otherwise it needs a rebalance
     */
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
    /**
     *   parent       parent  <-- rebalance here
     *     |            |
     *     y           [x]
     *    /      ->
     *  [x]
     *                 balance(parent) := ±1
     */
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
    /**
     *     |            |
     *     y            x  <-- rebalance here
     *    / \    ->    / \.
     *  [a]  x       [a] [b]
     *        \.
     *        [b]
     *               balance(x) := balance(y) - 1
     */
    void erase_node_pull_right(node_t* y) {
        node_t* x = y->link[1];
        node_t* parent = y->parent;
        bool y_is_right = y == parent->link[1];
        adopt_node(parent, x, y_is_right);
        adopt_node(x, y->link[0], 0);
        x->balance = y->balance - 1;
        rebalance_after_erase(x);
    }
    /**
     *        |                       |
     *        y                       x
     *       / \                     / \.
     *     [a]  x1                 [a]  x1
     *         / \                     / \.
     *       ... [b]      ->         ... [b]
     *       / \                     / \.
     *      w  [c]   rebalance -->  w  [c]
     *     / \          here       / \.
     *    x  [d]                 [e]  [d]
     *     \.
     *     [e]
     *                       balance(x) := balance(y)
     *                       balance(w) := balance(w) + 1
     */
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
        assert(parent && y && y->data && !y->parent);
        adopt_node(parent, y, is_right);
        rebalance_after_insert(y);
        node_count++;
    }
    void insert_node_after(node_t* node, node_t* y) {
        if (node->link[1]) {
            node_t* next = node_t::increment(node);
            insert_node(node_t::minimum(next), y, 0);
        } else {
            insert_node(node, y, 1);
        }
    }
    void insert_node_before(node_t* node, node_t* y) {
        if (node->link[0]) {
            node_t* prev = node_t::decrement(node);
            insert_node(node_t::maximum(prev), y, 1);
        } else {
            insert_node(node, y, 0);
        }
    }

    void erase_node(node_t* y) {
        assert(y && y->data && y->parent);
        erase_node_and_rebalance(y);
        drop_node(y);
        node_count--;
    }

    std::pair<iterator, bool> insert_node_unique(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = do_compare(*node->data, *y->data);
            if (!lesser && !do_compare(*y->data, *node->data)) {
                drop_node(node);
                return {iterator(y), false};
            }
            parent = y;
            y = y->link[!lesser];
        }
        insert_node(parent, node, !lesser);
        return {iterator(node), true};
    }
    iterator insert_node_multi(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = do_compare(*node->data, *y->data);
            parent = y;
            y = y->link[!lesser];
        }
        insert_node(parent, node, !lesser);
        return iterator(node);
    }
    iterator insert_node_hint_unique(node_t* node, node_t* hint) {
        if (hint == head) {
            if (node_count > 0 && do_compare(*maximum()->data, *node->data)) {
                insert_node(maximum(), node, 1);
                return iterator(node);
            }
            return insert_node_unique(node).first; // bad hint
        } else if (do_compare(*node->data, *hint->data)) {
            if (hint == minimum()) {
                insert_node(minimum(), node, 0);
                return iterator(node);
            }
            node_t* prev = node_t::decrement(hint);
            if (do_compare(*prev->data, *node->data)) {
                insert_node_before(hint, node);
                return iterator(node);
            }
            return insert_node_unique(node).first; // bad hint
        } else if (do_compare(*hint->data, *node->data)) {
            return insert_node_unique(node).first; // bad hint
        } else {
            drop_node(node);
            return iterator(hint);
        }
    }
    iterator insert_node_hint_multi(node_t* node, node_t* hint) {
        if (hint == head) {
            if (node_count > 0 && do_compare(*maximum()->data, *node->data)) {
                insert_node(maximum(), node, 1);
                return iterator(node);
            }
            return insert_node_multi(node); // bad hint
        } else if (do_compare(*node->data, *hint->data)) {
            if (hint == minimum()) {
                insert_node(minimum(), node, 0);
                return iterator(node);
            }
            node_t* prev = node_t::decrement(hint);
            if (do_compare(*prev->data, *node->data)) {
                insert_node_before(hint, node);
                return iterator(node);
            }
            return insert_node_multi(node);
        } else if (do_compare(*hint->data, *node->data)) {
            return insert_node_multi(node);
        } else {
            insert_node_before(hint, node);
            return iterator(node);
        }
    }

    std::pair<iterator, bool> insert_unique(const T& data) {
        node_t* node = new node_t(data);
        return insert_node_unique(node);
    }
    iterator insert_multi(const T& data) {
        node_t* node = new node_t(data);
        return insert_node_multi(node);
    }
    iterator insert_hint_unique(const_iterator hint, const T& data) {
        node_t* node = new node_t(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node);
    }
    iterator insert_hint_multi(const_iterator hint, const T& data) {
        node_t* node = new node_t(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace_unique(Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        return insert_node_unique(node);
    }
    template <typename... Args>
    iterator emplace_multi(Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        return insert_node_multi(node);
    }
    template <typename... Args>
    iterator emplace_hint_unique(const_iterator hint, Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node);
    }
    template <typename... Args>
    iterator emplace_hint_multi(const_iterator hint, Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }

    bool erase_unique(const T& data) {
        node_t* y = find_node(data);
        if (y != head) {
            erase_node(y);
            return true;
        }
        return false;
    }
    void erase(iterator pos) {
        assert(pos.y != head);
        erase_node(pos.y);
    }
    void erase(iterator first, iterator last) {
        for (auto it = first; it != last; ++it) {
            erase_node(it.y);
        }
    }

#if DEBUG_AVL_TREE
  private:
    void debug_print(const node_t* y, char S, int depth) const {
        std::string pad(2 * depth, ' ');
        if (!y) {
            std::cerr << pad << S << '\n';
            return;
        }
        std::cerr << pad << S << ' ' << *y->data << " (" << y->balance << ")\n";
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
        assert(y->data != nullptr);
        if (side == '<') {
            assert(!do_compare(*parent->data, *y->data));
        } else if (side == '>') {
            assert(!do_compare(*y->data, *parent->data));
        }
        int left = debug(y->link[0], y, '<');
        int right = debug(y->link[1], y, '>');
        if (y->balance != right - left) {
            std::cerr << "BUG balance at " << *y->data << "(" << y->balance << ")\n\n";
            debug_print();
        }
        assert(-1 <= y->balance && y->balance <= +1);
        assert(y->balance == right - left);
        return 1 + std::max(left, right);
    }

  public:
    void debug_print() const {
        std::fprintf(stderr, "====== nodes: %03lu ===========\n", node_count);
        debug_print(head->link[0], ' ', 0);
        std::fprintf(stderr, "=============================\n");
    }

    void debug() const {
        assert(head && !head->link[1] && !head->data && head->balance == 0);
        debug(head->link[0], head, 'R');
    }
#else
    void debug() const {}
#endif
};
