#include <bits/stdc++.h>
using namespace std;
static_assert(sizeof(int) == 4 && sizeof(long) == 8);

namespace cses {

template <typename T>
struct avl_node {
    using node_t = avl_node<T>;
    template <typename Tree>
    friend struct tree_debugger;

    node_t* parent = nullptr;
    node_t* link[2] = {};
    union {
        int8_t _dummy;
        T data;
    };
    int8_t balance = 0;

    ~avl_node() {
        delete link[0];
        delete link[1];
        if (this != parent)
            data.~T();
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
        while (n == n->parent->link[1])
            n = n->parent;
        return n->parent;
    }
    static const node_t* increment(const node_t* n) noexcept {
        if (n->link[1])
            return minimum(n->link[1]);
        while (n == n->parent->link[1])
            n = n->parent;
        return n->parent;
    }
    static node_t* decrement(node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }
    static const node_t* decrement(const node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }

  private:
    avl_node(const avl_node&) = delete;
    avl_node(avl_node&&) = delete;
    avl_node& operator=(const avl_node&) = delete;
    avl_node& operator=(avl_node&&) = delete;

    // hide this to prevent default-constructed data from creating head nodes
    struct avl_head_tag_t {};
    avl_node(avl_head_tag_t _tag) : parent(this) { (void)_tag; }

    avl_node(T data) : data(std::move(data)) {}
    template <typename... Args>
    avl_node(Args&&... args) : data(std::forward<Args>(args)...) {}

  public:
    static node_t* new_empty() { return new avl_node(avl_head_tag_t{}); }

    template <typename... Args>
    static node_t* make(Args&&... args) {
        return new avl_node(std::forward<Args>(args)...);
    }
};

/**
 * AVL binary search tree core
 * Tree is completely open
 */
template <typename T>
struct avl_tree {
    using node_t = avl_node<T>;
    template <typename Tree>
    friend struct tree_debugger;

    // The real tree's root is head->link[0]. head is never nullptr.
    node_t* head;
    node_t* min_node;
    node_t* max_node;
    size_t node_count;

    avl_tree() noexcept
        : head(node_t::new_empty()), min_node(head), max_node(head), node_count(0) {}

    // Move constructor
    avl_tree(avl_tree&& other) noexcept : avl_tree() { swap(other); }
    // Copy constructor
    avl_tree(const avl_tree& other) noexcept
        : head(node_t::new_empty()), node_count(other.node_count) {
        adopt_node(head, deep_clone_node(other.head->link[0]), 0);
        update_minmax();
    }
    // Move assignment
    avl_tree& operator=(avl_tree&& other) noexcept {
        if (head != other.head) {
            clear();
            swap(other);
        }
        return *this;
    }
    // Copy assignment
    avl_tree& operator=(const avl_tree& other) noexcept {
        if (head != other.head) {
            delete head->link[0];
            adopt_node(head, deep_clone_node(other.head->link[0]), 0);
            update_minmax();
            node_count = other.node_count;
        }
        return *this;
    }

    ~avl_tree() noexcept { delete head; }

    void clear() noexcept {
        delete head->link[0];
        head->link[0] = nullptr;
        min_node = max_node = head;
        node_count = 0;
    }
    void swap(avl_tree& other) noexcept {
        std::swap(head, other.head);
        std::swap(min_node, other.min_node);
        std::swap(max_node, other.max_node);
        std::swap(node_count, other.node_count);
    }
    friend void swap(avl_tree& lhs, avl_tree& rhs) noexcept { lhs.swap(rhs); }

    node_t* minimum() noexcept { return min_node; }
    const node_t* minimum() const noexcept { return min_node; }
    node_t* maximum() noexcept { return max_node; }
    const node_t* maximum() const noexcept { return max_node; }

  private:
    void update_minmax() {
        if (head->link[0]) {
            min_node = node_t::minimum(head->link[0]);
            max_node = node_t::maximum(head->link[0]);
        } else {
            min_node = max_node = head;
        }
    }
    static void drop_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        delete node;
    }
    static void adopt_node(node_t* parent, node_t* child, bool side) {
        parent->link[side] = child;
        if (child)
            child->parent = parent;
    }
    static void clear_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        node->parent = node;
        node->balance = 0;
    }
    static node_t* deep_clone_node(const node_t* node) {
        if (!node)
            return nullptr;
        node_t* clone = node_t::make(node->data);
        clone->balance = node->balance;
        adopt_node(clone, deep_clone_node(node->link[0]), 0);
        adopt_node(clone, deep_clone_node(node->link[1]), 1);
        return clone;
    }

    /**
     *       y                     x
     *      / \                   / \
     *    [a]  x        ->       y  [c]
     *        / \               / \
     *      [b] [c]           [a] [b]
     */
    node_t* rotate_left(node_t* y) {
        node_t* x = y->link[1];
        assert(y->balance >= +1 && y->balance >= x->balance);
        bool yside = y == y->parent->link[1];
        adopt_node(y->parent, x, yside);
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
    node_t* rotate_right(node_t* y) {
        node_t* x = y->link[0];
        assert(y->balance <= -1 && y->balance <= x->balance);
        bool yside = y == y->parent->link[1];
        adopt_node(y->parent, x, yside);
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
        if (y->balance == -2) {
            if (y->link[0]->balance == +1) {
                rotate_left(y->link[0]);
            }
            return rotate_right(y);
        }
        if (y->balance == +2) {
            if (y->link[1]->balance == -1) {
                rotate_right(y->link[1]);
            }
            return rotate_left(y);
        }
        return y;
    }

    /**
     *             p(-1)                     p(0)                       p(+1)
     *  h+1->h    / \   h         h->h-1    / \    h         h-1->h-2  / \   h
     *           /   \                     /   \                      /   \
     *          l     r                   l     r                    l     r
     *         / \   / \                 / \   / \                  / \   / \
     *       -.. .. .. ..              -.. .. .. ..               -.. .. .. ..
     * height lower in left       height lower in left    height lower in left
     *  new balance: 0             new balance: +1         new balance: +2 -> rebalance!
     *  delta height: h+1->h       delta height: h->h      delta height: 0 if new root
     *  overall height decreased   overall height did not  is imperfectly balanced (stop)
     *  continue on p's parent.    change so stop.         otherwise -1 so continue.
     */
    void rebalance_after_erase(node_t* y) {
        if (y == head)
            return;
        y = rebalance(y);
        while (y->parent != head && y->balance == 0) {
            bool yside = y == y->parent->link[1];
            y->parent->balance += yside ? -1 : +1;
            y = rebalance(y->parent);
        }
    }

    /**
     *            p(+1)                     p(0)                        p(-1)
     * h-1->h    / \   h         h->h+1    / \    h          h+1->h+2  / \   h
     *          /   \                     /   \                       /   \
     *         l     r                   l     r                     l     r
     *        / \   / \                 / \   / \                   / \   / \
     *      +.. .. .. ..              +.. .. .. ..                +.. .. .. ..
     * height higher in left   height higher in left     height higher in left
     *  new balance: 0          new balance: -1           new balance: -2 -> rebalance!
     *  delta height: h->h      delta height: h->h+1      delta height: h+1->h+1 (always)
     *  overall height did not  overall height increased  overall height did not
     *  change so stop.         continue on p's parent    change so stop.
     */
    void rebalance_after_insert(node_t* y) {
        while (y->parent != head && y->parent->balance == 0) {
            bool yside = y == y->parent->link[1];
            y->parent->balance = yside ? +1 : -1;
            y = y->parent;
        }
        if (y->parent != head) {
            bool yside = y == y->parent->link[1];
            y->parent->balance += yside ? +1 : -1;
            rebalance(y->parent);
        }
    }

    /**
     *   parent       parent  <-- rebalance here
     *     |            |
     *     y     ->    [x]
     *    /
     *  [x]
     *                 balance(parent) := ±1
     */
    void erase_node_pull_left(node_t* y) {
        node_t* x = y->link[0];
        node_t* parent = y->parent;
        bool yside = y == y->parent->link[1];
        adopt_node(parent, x, yside);
        if (parent != head) {
            parent->balance += yside ? -1 : +1;
            rebalance_after_erase(parent);
        }
    }

    /**
     *     |            |
     *     y            x  <-- rebalance here
     *    / \    ->    / \
     *  [a]  x       [a] [b]
     *        \
     *        [b]
     *               balance(x) := balance(y) - 1
     */
    void erase_node_pull_right(node_t* y) {
        node_t* x = y->link[1];
        node_t* parent = y->parent;
        bool yside = y == parent->link[1];
        adopt_node(parent, x, yside);
        adopt_node(x, y->link[0], 0);
        x->balance = y->balance - 1;
        rebalance_after_erase(x);
    }

    /**
     *        |                       |
     *        y                       x
     *       / \                     / \
     *     [a]  x1                 [a]  x1
     *         / \                     / \
     *       ... [b]      ->         ... [b]
     *       / \                     / \
     *      w  [c]   rebalance -->  w  [c]
     *     / \          here       / \
     *    x  [d]                 [e]  [d]
     *     \
     *     [e]
     *                       balance(x) := balance(y)
     *                       balance(w) := balance(w) + 1
     */
    void erase_node_minimum(node_t* y) {
        node_t* x = node_t::minimum(y->link[1]->link[0]);
        node_t* w = x->parent;
        node_t* parent = y->parent;
        bool yside = y == parent->link[1];
        adopt_node(parent, x, yside);
        adopt_node(w, x->link[1], 0);
        adopt_node(x, y->link[0], 0);
        adopt_node(x, y->link[1], 1);
        x->balance = y->balance;
        w->balance += 1;
        rebalance_after_erase(w);
    }

    /**
     * Select the erase position based on y's right subtree
     */
    void erase_node_and_rebalance(node_t* y) {
        if (!y->link[1])
            erase_node_pull_left(y);
        else if (!y->link[1]->link[0])
            erase_node_pull_right(y);
        else
            erase_node_minimum(y);
    }

    /**
     * Update min max pointers before insert
     */
    void insert_minmax(node_t* parent, node_t* y, bool side) {
        if (node_count > 0) {
            if (!side && parent == min_node)
                min_node = y;
            else if (side && parent == max_node)
                max_node = y;
        } else {
            min_node = max_node = y;
        }
    }

    /**
     * Update min max pointers before erase
     */
    void erase_minmax(node_t* y) {
        if (node_count > 1) {
            if (y == min_node)
                min_node = y->link[1] ? y->link[1] : y->parent;
            else if (y == max_node)
                max_node = y->link[0] ? y->link[0] : y->parent;
        } else {
            min_node = max_node = head;
        }
    }

  public:
    /**
     * Insert node y into the tree as a child of parent on the given side.
     * parent must not have a child on that side and y must be a free node.
     *
     *    parent         parent                 parent        parent
     *     /       ->     /  \         or           \    ->    /  \
     *   [l]            [l]   y                     [r]       y   [r]
     */
    void insert_node(node_t* parent, node_t* y, bool side) {
        insert_minmax(parent, y, side);
        adopt_node(parent, y, side);
        rebalance_after_insert(y);
        node_count++;
    }

    /**
     * Insert node y after node, so that incrementing node afterwards gives y.
     * Usually this will insert y as the right child of node.
     * y must be a free node.
     *
     *    parent         parent                 parent        parent
     *     /       ->     /  \         or        /  \    ->    /  \
     *   [l]            [l]   y                [l]   r       [l]   r
     *                                              /             /
     *                                            ...           ...
     *                                            /             /
     *                              ++parent --> x             x
     *                                            \           / \
     *                                            [r]        y  [r]
     */
    void insert_node_after(node_t* node, node_t* y) {
        if (node->link[1]) {
            insert_node(node_t::increment(node), y, 0);
        } else {
            insert_node(node, y, 1);
        }
    }

    /**
     * Insert node y before node, so that decrementing node afterwards gives y.
     * Usually this will insert y as the left child of node.
     * y must be a free node.
     *
     *   parent        parent                  parent        parent
     *       \    ->    /  \          or        /  \    ->    /  \
     *       [r]       y   [r]                 l   [r]       l   [r]
     *                                          \             \
     *                                          ...           ...
     *                                            \             \
     *                                --parent --> x             x
     *                                            /             / \
     *                                          [l]           [l]  y
     */
    void insert_node_before(node_t* node, node_t* y) {
        if (node->link[0]) {
            insert_node(node_t::decrement(node), y, 1);
        } else {
            insert_node(node, y, 0);
        }
    }

    /**
     * Remove node y from the tree and destroy it.
     */
    void erase_node(node_t* y) {
        erase_minmax(y);
        erase_node_and_rebalance(y);
        drop_node(y);
        node_count--;
    }

    /**
     * Remove node y from the tree but do not destroy it.
     */
    void yank_node(node_t* y) {
        erase_minmax(y);
        erase_node_and_rebalance(y);
        clear_node(y);
        node_count--;
    }
};

template <typename T>
using Tree = avl_tree<T>;

/**
 * set, multiset, map, multimap tags and traits for bs_tree and friends.
 */
enum bs_tree_tag { set_tag, map_tag };

/**
 * Forward declarations
 */
template <typename T, typename Compare, bs_tree_tag tag>
struct bst_traits;

template <typename T, typename Compare, bs_tree_tag tag>
struct bs_tree;

template <typename T, bs_tree_tag tag>
struct bst_node_handle_methods;

template <typename T>
struct bst_iterator;

template <typename T>
struct bst_const_iterator;

template <typename T, bs_tree_tag tag>
struct bst_node_handle;

template <typename T, bs_tree_tag tag>
struct bst_insert_return_type;

template <typename BSTree>
struct bst_inserter_unique_iterator;

template <typename BSTree>
struct bst_inserter_multi_iterator;

/**
 * Define BST traits for sets and maps
 * Sets store immutable elements themselves
 * Maps store pairs where the key is immutable but the mapped value is mutable
 */
template <typename Key, typename Compare>
struct bst_traits<Key, Compare, set_tag> {
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;

    using reference = const value_type&;
    using const_reference = const value_type&;
    using pointer = const value_type*;
    using const_pointer = const value_type*;

  protected:
    static constexpr inline const Key& get_key(const value_type& elem) noexcept {
        return elem;
    }
};
template <typename Key, typename T, typename Compare>
struct bst_traits<std::pair<const Key, T>, Compare, map_tag> {
    using key_type = Key;
    using value_type = std::pair<const Key, T>;
    using mapped_type = T;
    using key_compare = Compare;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

  protected:
    static constexpr inline const Key& get_key(const value_type& elem) noexcept {
        return elem.first;
    }
};

/**
 * Node handle methods:
 *   value(), key() and mapped()
 * needs specialization according to standard
 */
template <typename T>
struct bst_node_handle_methods<T, set_tag> {
    using value_type = T;

    value_type& value() const noexcept { return bst_node_handle<T, set_tag>::y->data; }
};

template <typename K, typename V>
struct bst_node_handle_methods<std::pair<const K, V>, map_tag> {
    using key_type = K;
    using mapped_type = V;

    key_type& key() const {
        return const_cast<key_type&>(bst_node_handle<V, map_tag>::y->data.first);
    }
    mapped_type& mapped() const { return bst_node_handle<V, map_tag>::y->data.second; }
};

/**
 * Non-const iterator for bs_tree
 * Works for all 4 container types
 */
template <typename T>
struct bst_iterator {
  protected:
    template <typename V, typename Compare, bs_tree_tag tag>
    friend struct bs_tree;
    friend bst_const_iterator<T>;
    using node_t = typename Tree<T>::node_t;
    using self_t = bst_iterator<T>;
    node_t* y;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = ptrdiff_t;

    bst_iterator() : y(nullptr) {}
    explicit bst_iterator(node_t* n) : y(n) {}

    explicit operator bool() const noexcept { return y != nullptr; }
    reference operator*() const noexcept { return y->data; }
    pointer operator->() const noexcept { return &y->data; }
    self_t& operator++() noexcept {
        y = node_t::increment(y);
        return *this;
    }
    self_t operator++(int) noexcept {
        self_t z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_t& operator--() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_t operator--(int) noexcept {
        self_t z = *this;
        y = node_t::decrement(y);
        return z;
    }
    friend bool operator==(const self_t& lhs, const self_t& rhs) noexcept {
        return lhs.y == rhs.y;
    }
    friend bool operator!=(const self_t& lhs, const self_t& rhs) noexcept {
        return lhs.y != rhs.y;
    }
};

/**
 * Const iterator for bs_tree
 * Works for all 4 container types
 */
template <typename T>
struct bst_const_iterator {
  protected:
    template <typename V, typename Compare, bs_tree_tag tag>
    friend struct bs_tree;
    friend bst_iterator<T>;
    using node_t = typename Tree<T>::node_t;
    using self_t = bst_const_iterator<T>;
    const node_t* y;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using difference_type = ptrdiff_t;

    bst_const_iterator() : y(nullptr) {}
    explicit bst_const_iterator(const node_t* n) : y(n) {}
    bst_const_iterator(bst_iterator<T> it) : y(it.y) {}

    explicit operator bool() const noexcept { return y != nullptr; }
    reference operator*() const noexcept { return y->data; }
    pointer operator->() const noexcept { return &y->data; }
    self_t& operator++() noexcept {
        y = node_t::increment(y);
        return *this;
    }
    self_t operator++(int) noexcept {
        self_t z = *this;
        y = node_t::increment(y);
        return z;
    }
    self_t& operator--() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_t operator--(int) noexcept {
        self_t z = *this;
        y = node_t::decrement(y);
        return z;
    }
    friend bool operator==(const self_t& lhs, const self_t& rhs) noexcept {
        return lhs.y == rhs.y;
    }
    friend bool operator!=(const self_t& lhs, const self_t& rhs) noexcept {
        return lhs.y != rhs.y;
    }
};

template <typename T>
struct bst_reverse_iterator : bst_iterator<T> {
  private:
    using node_t = typename Tree<T>::node_t;
    using self_t = bst_reverse_iterator<T>;
    using forward_t = bst_iterator<T>;
    using forward_t::y;

  public:
    using forward_t::forward_t;
    bst_reverse_iterator(forward_t it) : forward_t(it.y) {}

    self_t& operator++() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_t operator++(int) noexcept {
        self_t z = *this;
        y = node_t::decrement(y);
        return z;
    }
    self_t& operator--() noexcept {
        y = node_t::increment(y);
        return *this;
    }
    self_t operator--(int) noexcept {
        self_t z = *this;
        y = node_t::increment(y);
        return z;
    }
};

template <typename T>
struct bst_const_reverse_iterator : private bst_const_iterator<T> {
  private:
    using node_t = typename Tree<T>::node_t;
    using self_t = bst_const_reverse_iterator<T>;
    using forward_t = bst_const_iterator<T>;
    using forward_t::y;

  public:
    using forward_t::forward_t;
    bst_const_reverse_iterator(forward_t it) : forward_t(it.y) {}

    self_t& operator++() noexcept {
        y = node_t::decrement(y);
        return *this;
    }
    self_t operator++(int) noexcept {
        self_t z = *this;
        y = node_t::decrement(y);
        return z;
    }
    self_t& operator--() noexcept {
        y = node_t::increment(y);
        return *this;
    }
    self_t operator--(int) noexcept {
        self_t z = *this;
        y = node_t::increment(y);
        return z;
    }
};

/**
 * The handle abstraction for extracted nodes
 */
template <typename T, bs_tree_tag tag>
struct bst_node_handle : bst_node_handle_methods<T, tag> {
  private:
    template <typename V, typename Compare, bs_tree_tag bstag>
    friend struct bs_tree;
    using node_t = typename Tree<T>::node_t;
    using self_t = bst_node_handle<T, tag>;
    node_t* y;

  public:
    bst_node_handle() : y(nullptr) {}
    explicit bst_node_handle(node_t* n) : y(n) {}

    bst_node_handle(self_t&& other) noexcept : y(other.y) { other.y = nullptr; }
    bst_node_handle& operator=(self_t&& other) noexcept {
        delete y;
        y = other.y;
        other.y = nullptr;
    }

    ~bst_node_handle() noexcept { delete y; }

    bool empty() const noexcept { return y == nullptr; }
    explicit operator bool() const noexcept { return y != nullptr; }
    void swap(self_t& other) noexcept { swap(y, other.y); }
    friend void swap(self_t& lhs, self_t& rhs) noexcept { lhs.swap(rhs); }
};

/**
 * Return type for unique insertion using node handles
 */
template <typename T, bs_tree_tag tag>
struct bst_insert_return_type {
    using iterator = bst_iterator<T>;
    using node_type = bst_node_handle<T, tag>;

    iterator position;
    bool inserted;
    node_type node;
};

/**
 * Binary search tree built on top of the selected tree core
 * Can generate all 4 types of containers (set, multiset, map and multimap)
 */
template <typename T, typename Compare = std::less<T>, bs_tree_tag tag = set_tag>
struct bs_tree : private Tree<T>, public bst_traits<T, Compare, tag> {
  private:
    using Traits = bst_traits<T, Compare, tag>;
    using node_t = typename Tree<T>::node_t;

    template <typename BSTree>
    friend struct tree_debugger;

    using Traits::get_key;
    using Tree<T>::head;
    using Tree<T>::node_count;
    using Tree<T>::insert_node;
    using Tree<T>::insert_node_after;
    using Tree<T>::insert_node_before;
    using Tree<T>::erase_node;
    using Tree<T>::yank_node;
    using Tree<T>::maximum;
    using Tree<T>::minimum;

  protected:
    using Key = typename Traits::key_type;
    Compare comp;

    template <typename K1 = Key, typename K2 = Key>
    inline bool compare(const K1& lhs, const K2& rhs) const noexcept {
        return comp(lhs, rhs);
    }

  public:
    const Tree<T>& internal() const { return *this; }

    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;

    using iterator = bst_iterator<T>;
    using const_iterator = bst_const_iterator<T>;
    using reverse_iterator = bst_reverse_iterator<T>;
    using const_reverse_iterator = bst_const_reverse_iterator<T>;

    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using reference = typename Traits::reference;
    using const_reference = typename Traits::const_reference;
    using pointer = typename Traits::pointer;
    using const_pointer = typename Traits::const_pointer;

    using node_type = bst_node_handle<T, tag>;
    using insert_return_type = bst_insert_return_type<T, tag>;

    bs_tree() = default;
    explicit bs_tree(const Compare& comp) : comp(comp) {}

    bs_tree(const bs_tree& other) = default;
    bs_tree(bs_tree&& other) = default;
    bs_tree& operator=(const bs_tree& other) = default;
    bs_tree& operator=(bs_tree&& other) = default;

    using Tree<T>::clear;

    inline size_type size() const noexcept { return node_count; }
    inline bool empty() const noexcept { return node_count == 0; }
    constexpr size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max();
    }

    void swap(bs_tree& other) {
        using std::swap;
        Tree<T>::swap(other);
        swap(comp, other.comp);
    }
    friend inline void swap(bs_tree& lhs, bs_tree& rhs) noexcept { lhs.swap(rhs); }

    inline iterator begin() noexcept { return iterator(minimum()); }
    inline iterator end() noexcept { return iterator(head); }
    inline const_iterator begin() const noexcept { return const_iterator(minimum()); }
    inline const_iterator end() const noexcept { return const_iterator(head); }
    inline const_iterator cbegin() const noexcept { return const_iterator(minimum()); }
    inline const_iterator cend() const noexcept { return const_iterator(head); }
    inline reverse_iterator rbegin() noexcept { return reverse_iterator(maximum()); }
    inline reverse_iterator rend() noexcept { return reverse_iterator(head); }
    inline const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(maximum());
    }
    inline const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(head);
    }
    inline const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(maximum());
    }
    inline const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(head);
    }

    friend bool operator==(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return lhs.size() == rhs.size() &&
               std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    friend bool operator!=(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return !(lhs == rhs);
    }
    friend bool operator<(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                            rhs.end());
    }
    friend bool operator>(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return rhs < lhs;
    }
    friend bool operator<=(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return !(rhs < lhs);
    }
    friend bool operator>=(const bs_tree& lhs, const bs_tree& rhs) noexcept {
        return !(lhs < rhs);
    }

  private:
    template <typename K = Key>
    node_t* find_node(const K& key) {
        node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(key, get_key(x->data));
            if (!lesser && !compare(get_key(x->data), key))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = Key>
    const node_t* find_node(const K& key) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(key, get_key(x->data));
            if (!lesser && !compare(get_key(x->data), key))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = Key>
    node_t* lower_bound_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (!compare(get_key(x->data), key))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = Key>
    const node_t* lower_bound_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (!compare(get_key(x->data), key))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = Key>
    node_t* upper_bound_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (compare(key, get_key(x->data)))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = Key>
    const node_t* upper_bound_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (compare(key, get_key(x->data)))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = Key>
    std::pair<node_t*, node_t*> equal_range_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (compare(get_key(x->data), key))
                x = x->link[1];
            else if (compare(key, get_key(x->data)))
                y = x, x = x->link[0];
            else {
                node_t* xu = x->link[1];
                node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!compare(get_key(x->data), key))
                        y = x, x = x->link[0];
                    else
                        x = x->link[1];
                }
                // upper bound [xu, yu]
                while (xu) {
                    if (compare(key, get_key(xu->data)))
                        yu = xu, xu = xu->link[0];
                    else
                        xu = xu->link[1];
                }
                return {y, yu};
            }
        }
        return {y, y};
    }
    template <typename K = Key>
    std::pair<const node_t*, const node_t*> equal_range_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (compare(get_key(x->data), key))
                x = x->link[1];
            else if (compare(key, get_key(x->data)))
                y = x, x = x->link[0];
            else {
                const node_t* xu = x->link[1];
                const node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!compare(get_key(x->data), key))
                        y = x, x = x->link[0];
                    else
                        x = x->link[1];
                }
                // upper bound [xu, yu]
                while (xu) {
                    if (compare(key, xu->data))
                        yu = xu, xu = xu->link[0];
                    else
                        xu = xu->link[1];
                }
                return {y, yu};
            }
        }
        return {y, y};
    }

  public:
    template <typename K = Key>
    iterator find(const K& key) {
        return iterator(find_node(key));
    }
    template <typename K = Key>
    const_iterator find(const K& key) const {
        return const_iterator(find_node(key));
    }
    template <typename K = Key>
    iterator lower_bound(const K& key) {
        return iterator(lower_bound_node(key));
    }
    template <typename K = Key>
    const_iterator lower_bound(const K& key) const {
        return const_iterator(lower_bound_node(key));
    }
    template <typename K = Key>
    iterator upper_bound(const K& key) {
        return iterator(upper_bound_node(key));
    }
    template <typename K = Key>
    const_iterator upper_bound(const K& key) const {
        return const_iterator(upper_bound_node(key));
    }
    template <typename K = Key>
    std::pair<iterator, iterator> equal_range(const K& key) {
        auto nodes = equal_range_node(key);
        return {iterator(nodes.first), iterator(nodes.second)};
    }
    template <typename K = Key>
    std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
        auto nodes = equal_range_node(key);
        return {const_iterator(nodes.first), const_iterator(nodes.second)};
    }
    template <typename K = Key>
    bool contains(const K& key) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(key, get_key(x->data));
            if (!lesser && !compare(get_key(x->data), key))
                return true;
            x = x->link[!lesser];
        }
        return false;
    }
    template <typename K = Key>
    size_t count(const K& key) const {
        auto range = equal_range(key);
        return std::distance(range.first, range.second);
    }

  private:
    std::pair<iterator, bool> try_insert_node_unique(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = compare(get_key(node->data), get_key(y->data));
            if (!lesser && !compare(get_key(y->data), get_key(node->data)))
                return {iterator(y), false};
            parent = y;
            y = y->link[!lesser];
        }
        insert_node(parent, node, !lesser);
        return {iterator(node), true};
    }
    std::pair<iterator, bool> insert_node_unique(node_t* node) {
        auto res = try_insert_node_unique(node);
        if (!res.second)
            delete node;
        return res;
    }
    std::pair<iterator, bool> try_insert_node_hint_unique(node_t* node, node_t* hint) {
        const auto& key = get_key(node->data);
        const auto& hint_key = get_key(hint->data);
        if (hint == head) {
            if (size() == 0) {
                insert_node(hint, node, 0);
                return {iterator(node), true};
            }
            if (compare(get_key(maximum()->data), key)) {
                insert_node(maximum(), node, 1);
                return {iterator(node), true};
            } else if (compare(key, get_key(maximum()->data))) {
                return try_insert_node_unique(node); // bad hint
            } else {
                return {iterator(maximum()), false};
            }
        } else if (compare(key, hint_key)) {
            if (hint == minimum()) {
                insert_node(minimum(), node, 0);
                return {iterator(node), true};
            }
            node_t* prev = node_t::decrement(hint);
            if (compare(get_key(prev->data), key)) {
                insert_node_before(hint, node);
                return {iterator(node), true};
            }
            return try_insert_node_unique(node); // bad hint
        } else if (compare(hint_key, key)) {
            return try_insert_node_unique(node); // bad hint
        }
        return {iterator(hint), false};
    }
    std::pair<iterator, bool> insert_node_hint_unique(node_t* node, node_t* hint) {
        auto res = try_insert_node_hint_unique(node, hint);
        if (!res.second)
            delete node;
        return res;
    }
    iterator insert_node_multi(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = compare(get_key(node->data), get_key(y->data));
            parent = y;
            y = y->link[!lesser];
        }
        insert_node(parent, node, !lesser);
        return iterator(node);
    }
    iterator insert_node_hint_multi(node_t* node, node_t* hint) {
        const auto& key = get_key(node->data);
        const auto& hint_key = get_key(hint->data);
        if (hint == head) {
            if (size() == 0) {
                insert_node(hint, node, 0);
                return iterator(node);
            } else if (!compare(key, get_key(maximum()->data))) {
                insert_node(maximum(), node, 1);
                return iterator(node);
            }
            return insert_node_multi(node); // bad hint
        } else if (!compare(hint_key, key)) {
            if (hint == minimum()) {
                insert_node(minimum(), node, 0);
                return iterator(node);
            }
            node_t* prev = node_t::decrement(hint);
            if (!compare(key, get_key(prev->data))) {
                insert_node_before(hint, node);
                return iterator(node);
            }
            return insert_node_multi(node);
        } else if (!compare(key, hint_key)) {
            return insert_node_multi(node);
        }
        insert_node_before(hint, node);
        return iterator(node);
    }

  public:
    insert_return_type insert_unique(node_type&& nh) {
        if (nh.y == nullptr)
            return {end(), false, node_type()};
        auto res = try_insert_node_unique(nh.y);
        if (res.second)
            nh.y = nullptr;
        return {res.first, res.second, std::move(nh)};
    }
    iterator insert_hint_unique(const_iterator hint, node_type&& nh) {
        if (nh.y == nullptr)
            return end();
        node_t* hint_node = const_cast<node_t*>(hint.y);
        auto res = try_insert_node_hint_unique(nh.y, hint_node);
        if (res.second)
            nh.y = nullptr;
        return res.first;
    }
    std::pair<iterator, bool> insert_unique(const T& data) {
        node_t* node = node_t::make(data);
        return insert_node_unique(node);
    }
    iterator insert_hint_unique(const_iterator hint, const T& data) {
        node_t* node = node_t::make(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node).first;
    }
    std::pair<iterator, bool> insert_unique(T&& data) {
        node_t* node = node_t::make(std::move(data));
        return insert_node_unique(node);
    }
    iterator insert_hint_unique(const_iterator hint, T&& data) {
        node_t* node = node_t::make(std::move(data));
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node).first;
    }
    template <typename InputIt>
    void insert_unique(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insert_unique(*it);
        }
    }
    void insert_unique(std::initializer_list<T> ilist) {
        insert_unique(ilist.begin(), ilist.end());
    }

    iterator insert_multi(node_type&& nh) {
        auto it = insert_node_multi(nh.y);
        nh.y = nullptr;
        return it;
    }
    iterator insert_hint_multi(const_iterator hint, node_type&& nh) {
        if (nh.y == nullptr)
            return end();
        node_t* hint_node = const_cast<node_t*>(hint.y);
        auto it = insert_node_hint_multi(nh.y, hint_node);
        nh.y = nullptr;
        return it;
    }
    iterator insert_multi(const T& data) {
        node_t* node = node_t::make(data);
        return insert_node_multi(node);
    }
    iterator insert_hint_multi(const_iterator hint, const T& data) {
        node_t* node = node_t::make(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }
    iterator insert_multi(T&& data) {
        node_t* node = node_t::make(std::move(data));
        return insert_node_multi(node);
    }
    iterator insert_hint_multi(const_iterator hint, T&& data) {
        node_t* node = node_t::make(std::move(data));
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }
    template <typename InputIt>
    void insert_multi(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            insert_multi(*it);
        }
    }
    void insert_multi(std::initializer_list<T> ilist) {
        insert_multi(ilist.begin(), ilist.end());
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace_unique(Args&&... args) {
        node_t* node = node_t::make(std::forward<Args>(args)...);
        return insert_node_unique(node);
    }
    template <typename... Args>
    iterator emplace_hint_unique(const_iterator hint, Args&&... args) {
        node_t* node = node_t::make(std::forward<Args>(args)...);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node).first;
    }

    template <typename... Args>
    iterator emplace_multi(Args&&... args) {
        node_t* node = node_t::make(std::forward<Args>(args)...);
        return insert_node_multi(node);
    }
    template <typename... Args>
    iterator emplace_hint_multi(const_iterator hint, Args&&... args) {
        node_t* node = node_t::make(std::forward<Args>(args)...);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }

    bool erase_unique(const Key& key) {
        node_t* y = find_node(key);
        if (y != head) {
            erase_node(y);
            return true;
        }
        return false;
    }
    size_t erase_multi(const Key& key) {
        std::pair<iterator, iterator> range = equal_range(key);
        size_t s = size();
        for (auto it = range.first; it != range.second;) {
            node_t* y = it.y;
            ++it;
            erase_node(y);
        }
        return s - size();
    }
    void erase(iterator pos) {
        assert(pos.y != head);
        erase_node(pos.y);
    }
    void erase(iterator first, iterator last) {
        for (auto it = first; it != last;) {
            erase(it++);
        }
    }
    template <typename Pred>
    friend size_type erase_if(bs_tree& bst, Pred pred) {
        auto s = bst.size();
        for (auto it = bst.begin(), last = bst.end(); it != last;) {
            if (pred(*it))
                it = bst.erase(it);
            else
                ++it;
        }
        return s - bst.size();
    }

    node_type extract(const_iterator pos) {
        assert(pos.y != head);
        node_t* y = const_cast<node_t*>(pos.y);
        yank_node(y);
        return node_type(y);
    }
    node_type extract(const Key& key) {
        node_t* y = find_node(key);
        if (y != head) {
            yank_node(y);
            return node_type(y);
        }
        return node_type();
    }

  protected:
    template <typename CmpFn2>
    void merge_unique(bs_tree<T, CmpFn2, tag>& src) {
        for (auto it = src.begin(); it != src.end();) {
            node_t* node = it.y;
            ++it;
            node_t* x = head->link[0];
            node_t* y = head;
            bool lesser = true;
            while (x) {
                lesser = compare(get_key(node->data), get_key(x->data));
                if (!lesser && !compare(get_key(x->data), get_key(node->data)))
                    goto skip;
                y = x;
                x = x->link[!lesser];
            }
            src.yank_node(node);
            insert_node(y, node, !lesser);
        skip:;
        }
    }
    template <typename CmpFn2>
    void merge_unique(bs_tree<T, CmpFn2, tag>&& src) {
        merge_unique(src);
    }
    template <typename CmpFn2>
    void merge_multi(bs_tree<T, CmpFn2, tag>& src) {
        for (auto it = src.begin(); it != src.end();) {
            node_t* node = it.y;
            ++it;
            assert(node && node->parent);
            node_t* x = head->link[0];
            node_t* y = head;
            bool lesser = true;
            while (x) {
                lesser = compare(get_key(node->data), get_key(x->data));
                y = x;
                x = x->link[!lesser];
            }
            src.yank_node(node);
            insert_node(y, node, !lesser);
        }
    }
    template <typename CmpFn2>
    void merge_multi(bs_tree<T, CmpFn2, tag>&& src) {
        merge_multi(src);
    }
};

template <typename BSTree>
struct bst_inserter_unique_iterator {
  private:
    using T = typename BSTree::value_type;
    BSTree* tree;

  public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using self_t = bst_inserter_unique_iterator;
    using container_type = BSTree;

    bst_inserter_unique_iterator(container_type& tree) : tree(&tree) {}

    self_t& operator*() { return *this; }
    self_t& operator++() { return *this; }
    self_t& operator++(int) { return *this; }
    self_t& operator=(const T& value) {
        tree->insert_unique(value);
        return *this;
    }
    self_t& operator=(T&& value) {
        tree->insert_unique(std::move(value));
        return *this;
    }
};

template <typename BSTree>
struct bst_inserter_multi_iterator {
  private:
    using T = typename BSTree::value_type;
    BSTree* tree;

  public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using self_t = bst_inserter_multi_iterator;
    using container_type = BSTree;

    bst_inserter_multi_iterator(container_type& tree) : tree(&tree) {}

    self_t& operator*() { return *this; }
    self_t& operator++() { return *this; }
    self_t& operator++(int) { return *this; }
    self_t& operator=(const T& value) {
        tree->insert_multi(value);
        return *this;
    }
    self_t& operator=(T&& value) {
        tree->insert_multi(std::move(value));
        return *this;
    }
};

template <typename BSTree>
bst_inserter_unique_iterator<BSTree> bst_inserter_unique(BSTree& tree) {
    return bst_inserter_unique_iterator<BSTree>(tree);
}

template <typename BSTree>
bst_inserter_multi_iterator<BSTree> bst_inserter_multi(BSTree& tree) {
    return bst_inserter_multi_iterator<BSTree>(tree);
}

template <typename Key, typename Compare = std::less<Key>>
struct bs_set : bs_tree<Key, Compare, set_tag> {
  private:
    using bst = bs_tree<Key, Compare, set_tag>;
    using T = typename bst::value_type;

  public:
    using key_compare = typename bst::key_compare;
    using key_type = typename bst::key_type;
    using value_type = typename bst::value_type;
    using iterator = typename bst::iterator;
    using const_iterator = typename bst::const_iterator;
    using reverse_iterator = typename bst::reverse_iterator;
    using const_reverse_iterator = typename bst::const_reverse_iterator;
    using size_type = typename bst::size_type;
    using difference_type = typename bst::difference_type;
    using reference = typename bst::reference;
    using const_reference = typename bst::const_reference;
    using pointer = typename bst::pointer;
    using const_pointer = typename bst::const_pointer;
    using node_type = typename bst::node_type;
    using insert_return_type = typename bst::insert_return_type;

    using bst::bst;

    template <typename InputIt>
    bs_set(InputIt first, InputIt last) {
        bst::insert_unique(first, last);
    }

    template <typename InputIt>
    bs_set(InputIt first, InputIt last, const Compare& comp) : bst(comp) {
        bst::insert_unique(first, last);
    }

    bs_set(std::initializer_list<value_type> ilist) {
        bst::insert_unique(std::move(ilist));
    }

    bs_set(std::initializer_list<value_type> ilist, const Compare& comp) : bst(comp) {
        bst::insert_unique(std::move(ilist));
    }

    bs_set& operator=(std::initializer_list<value_type> ilist) {
        bst::clear();
        bst::insert_unique(std::move(ilist));
        return *this;
    }

    size_t count(const Key& key) const noexcept { return bst::contains(key); }

    insert_return_type insert(node_type&& nh) {
        return bst::insert_unique(std::move(nh));
    }
    iterator insert(const_iterator hint, node_type&& nh) {
        return bst::insert_hint_unique(hint, std::move(nh));
    }
    std::pair<iterator, bool> insert(const T& data) { return bst::insert_unique(data); }
    iterator insert(const_iterator hint, const T& data) {
        return bst::insert_hint_unique(hint, data);
    }
    std::pair<iterator, bool> insert(T&& data) {
        return bst::insert_unique(std::move(data));
    }
    iterator insert(const_iterator hint, T&& data) {
        return bst::insert_hint_unique(hint, std::move(data));
    }
    template <typename InputIt>
    void insert(InputIt first, InputIt last) {
        return bst::insert_unique(first, last);
    }
    void insert(std::initializer_list<T> ilist) {
        return bst::insert_unique(std::move(ilist));
    }
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return bst::emplace_unique(std::forward<Args>(args)...);
    }
    template <typename... Args>
    std::pair<iterator, bool> emplace_hint(const_iterator hint, Args&&... args) {
        return bst::emplace_hint_unique(hint, std::forward<Args>(args)...);
    }

    using bst::erase;
    bool erase(const Key& key) { return bst::erase_unique(key); }

    template <typename Compare2>
    void merge(bs_set<Key, Compare2>& src) {
        return bst::merge_unique(src);
    }
    template <typename Compare2>
    void merge(bs_set<Key, Compare2>&& src) {
        return bst::merge_unique(std::move(src));
    }

    friend bst_inserter_unique_iterator<bst> inserter(bs_set& set) {
        return bst_inserter_unique<bst>(set);
    }
};

} // namespace cses

using intset = cses::bs_set<int>;

vector<vector<int>> adj;
vector<int> color;
vector<int> answer;
int timer = 0;

void quick_merge(intset& a, intset&& b) {
    if (a.size() < b.size()) {
        swap(a, b);
    }
    a.merge(b);
}

intset dfs(int u, int p) {
    intset nums = {color[u]};
    for (int v : adj[u]) {
        if (v != p) {
            quick_merge(nums, dfs(v, u));
        }
    }
    answer[u] = nums.size();
    return nums;
}

int main() {
    ios::sync_with_stdio(false);
    int N;
    cin >> N;
    adj.resize(N + 1);
    color.resize(N + 1);
    answer.resize(N + 1);
    for (int u = 1; u <= N; u++) {
        cin >> color[u];
    }
    for (int i = 1; i < N; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    dfs(1, 0);

    for (int u = 1; u <= N; u++) {
        cout << answer[u] << " \n"[u == N];
    }
    return 0;
}
