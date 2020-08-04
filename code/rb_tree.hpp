#include <cassert>
#include <iostream>
#include <limits>
#include <memory>

#define RB_DEBUG(...) fprintf(stderr, __VA_ARGS__);
#define RB_ASSERT(condition)                                 \
    if (!(condition)) {                                      \
        RB_DEBUG("\ninvariant [" #condition "] violated\n"); \
        assert(condition);                                   \
    }

// *****

enum rb_color_t : int8_t { rb_red = 0, rb_black = 1 };

/**
 * Red-black node
 * This same class is used to represent the head node. The node is the tree's head
 * iff it does not hold data iff the parent pointer is this (itself).
 * We hide the head constructor to prevent default-constructed data from generating
 * head nodes
 */
template <typename T>
struct rb_node {
    using node_t = rb_node<T>;

    node_t* parent = nullptr;
    node_t* link[2] = {};
    union {
        int8_t _dummy;
        T data;
    };
    rb_color_t color = rb_red;

    rb_node(T data) : data(std::move(data)) {}
    template <typename... Args>
    rb_node(Args&&... args) : data(std::forward<Args>(args)...) {}

    ~rb_node() {
        delete link[0];
        delete link[1];
        if (this != parent)
            data.~T();
    }

    inline bool is_leaf() const noexcept {
        return !link[0] && !link[1];
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
        while (n->parent != n && n == n->parent->link[1])
            n = n->parent;
        return n->parent;
    }
    static const node_t* increment(const node_t* n) noexcept {
        if (n->link[1])
            return minimum(n->link[1]);
        while (n->parent != n && n == n->parent->link[1])
            n = n->parent;
        return n->parent;
    }
    static node_t* decrement(node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent != n && n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }
    static const node_t* decrement(const node_t* n) noexcept {
        if (n->link[0])
            return maximum(n->link[0]);
        while (n->parent != n && n == n->parent->link[0])
            n = n->parent;
        return n->parent;
    }

  private:
    rb_node(const rb_node&) = delete;
    rb_node(rb_node&&) = delete;
    rb_node& operator=(const rb_node&) = delete;
    rb_node& operator=(rb_node&&) = delete;

    struct rb_head_tag_t {};
    rb_node([[maybe_unused]] rb_head_tag_t tag) : parent(this) {}

  public:
    static node_t* new_empty() {
        return new node_t(rb_head_tag_t{});
    }
};

/**
 * Red-black binary search tree core
 * Tree is completely open
 */
template <typename T>
struct rb_tree {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using node_t = rb_node<T>;
    using tree_t = rb_tree<T>;

    // The real tree's root is head->link[0]. head is never nullptr.
    node_t* head;
    size_t node_count;

    rb_tree() noexcept : head(node_t::new_empty()), node_count(0) {}

    // Move constructor
    rb_tree(rb_tree&& other) noexcept
        : head(node_t::new_empty()), node_count(other.node_count) {
        adopt_node(head, other.head->link[0], 0);
        other.head->link[0] = nullptr;
        other.node_count = 0;
    }
    // Copy constructor
    rb_tree(const rb_tree& other) noexcept
        : head(node_t::new_empty()), node_count(other.node_count) {
        adopt_node(head, deep_clone_node(other.head->link[0]), 0);
    }
    // Move assignment
    rb_tree& operator=(rb_tree&& other) noexcept {
        delete head->link[0];
        adopt_node(head, other.head->link[0], 0);
        node_count = other.node_count;
        other.head->link[0] = nullptr;
        other.node_count = 0;
        return *this;
    }
    // Copy assignment
    rb_tree& operator=(const rb_tree& other) noexcept {
        delete head->link[0];
        adopt_node(head, deep_clone_node(other.head->link[0]), 0);
        node_count = other.node_count;
        return *this;
    }

    ~rb_tree() noexcept {
        delete head;
    }

    inline void swap(rb_tree& other) noexcept {
        std::swap(head, other.head);
        std::swap(node_count, other.node_count);
    }
    friend inline void swap(rb_tree& lhs, rb_tree& rhs) noexcept {
        lhs.swap(rhs);
    }
    inline void clear() noexcept {
        delete head->link[0];
        head->link[0] = nullptr;
        node_count = 0;
    }
    inline size_t size() const noexcept {
        return node_count;
    }
    inline bool empty() const noexcept {
        return node_count == 0;
    }
    constexpr size_t max_size() const noexcept {
        return std::numeric_limits<size_t>::max();
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

    static inline void drop_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        delete node;
    }
    static inline void drop_subtree(node_t* node) {
        delete node;
    }
    static inline void adopt_node(node_t* parent, node_t* child, bool is_right) {
        parent->link[is_right] = child;
        if (child)
            child->parent = parent;
    }
    static inline void clear_node(node_t* node) {
        node->link[0] = node->link[1] = nullptr;
        node->parent = nullptr;
        node->color = rb_red;
    }
    static node_t* deep_clone_node(const node_t* node) {
        if (!node)
            return nullptr;
        node_t* clone = new node_t(node->data);
        clone->color = node->color;
        adopt_node(clone, deep_clone_node(node->link[0]), 0);
        adopt_node(clone, deep_clone_node(node->link[1]), 1);
        return clone;
    }

    static inline bool is_black(node_t* node) noexcept {
        return !node || node->color == rb_black;
    }
    static inline bool is_red(node_t* node) noexcept {
        return node && node->color == rb_red;
    }
    static inline bool is_black_blob(node_t* node) noexcept {
        return node && node->color == rb_black && is_black(node->link[0]) &&
               is_black(node->link[1]);
    }

    /**
     *       y                     x
     *      / \                   / \
     *    [a]  x        ->       y  [c]
     *        / \               / \
     *      [b] [c]           [a] [b]
     */
    void left_rotate(node_t* y) {
        node_t* x = y->link[1];
        bool is_right = y == y->parent->link[1];
        adopt_node(y->parent, x, is_right);
        adopt_node(y, x->link[0], 1);
        adopt_node(x, y, 0);
    }
    /**
     *         y                  x
     *        / \                / \
     *       x  [c]     ->     [a]  y
     *      / \                    / \
     *    [a] [b]                [b] [c]
     */
    void right_rotate(node_t* y) {
        node_t* x = y->link[0];
        bool is_right = y == y->parent->link[1];
        adopt_node(y->parent, x, is_right);
        adopt_node(y, x->link[1], 0);
        adopt_node(x, y, 1);
    }
    /**
     *      _g_         _y_               _g_         _p_
     *     /   \       /   \             /   \       /   \
     *   (p)    u -> (p)   (g)         (p)    u -> (y)   (g)        (p)        (p)
     *   / \         / \   / \         / \         / \   / \        /  !yp       \  yp
     *  .. (y)      ..  a b   u      (y)  b       .. .. b   u      y              y
     *     / \                       / \
     *    a   b                     .. ..
     *         first if                      second if
     */
    node_t* rebalance(node_t* y) {
        node_t* p = y->parent;
        node_t* g = p->parent;
        bool yp = y == p->link[1];
        bool pg = p == g->link[1];

        assert(g->link[!pg]->color == rb_black); // the uncle of y is black
        g->color = rb_red;

        if (yp && !pg)
            return y->color = rb_black, left_rotate(p), right_rotate(g);
        else if (!yp && !pg)
            return p->color = rb_black, right_rotate(g);
        else if (yp)
            return p->color = rb_black, left_rotate(g);
        else
            return y->color = rb_black, right_rotate(p), left_rotate(g);
    }
    /**
     *      __g__             _(g)_             __g__              __y__
     *     /     \           /     \           /     \            /     \
     *   (p)     (u)   ->   p       u        (p)      u    ->   (p)     (g)
     *   / \     / \       / \     / \       / \     / \        / \     / \
     *  .. (y)  .. ..     .. (y)  .. ..     .. (y)  .. ..      .. ..   ..  u
     *         case 1: p=red, u=red                 case 2: u=black
     *  recolor p,u,g and continue on g     rotate around p and recolor y,g
     *
     *  whenever p is black we're done.
     */
    void rebalance_after_insert(node_t* y) {
        node_t* p = y->parent;
        node_t* g = p->parent;
        node_t* u = g->link[p != g->link[1]];
        assert(p && g);
        while (g != head && p->color == rb_red && is_red(u)) {
            p->color = rb_black, u->color = rb_black, g->color = rb_red;
            y = g, p = y->parent, g = p->parent, u = g->link[p != g->link[1]];
        }
        if (p == head)
            y->color = rb_black;
        else if (p->color == rb_red) {
            g->color = rb_red;
            bool yp = y == p->link[1];
            bool pg = p == g->link[1];

            if (yp && !pg)
                y->color = rb_black, left_rotate(p), right_rotate(g);
            else if (!yp && !pg)
                p->color = rb_black, right_rotate(g);
            else if (yp)
                p->color = rb_black, left_rotate(g);
            else
                y->color = rb_black, right_rotate(p), left_rotate(g);
        }
    }

    void rebalance_after_black_erase(node_t* p, bool nr) {
        node_t* n = p->link[nr];
        if (is_red(n))
            n->color = rb_black;
        else
            return delete_case1(p, nr);
    }

    void delete_case1(node_t* p, bool nr) {
        if (p != head)
            delete_case2(p, nr);
    }

    void delete_case2(node_t* p, bool nr) {
        node_t* s = p->link[!nr];
        if (is_red(s)) {
            p->color = rb_red;
            s->color = rb_black;
            nr ? right_rotate(p) : left_rotate(p);
        }
        delete_case3(p, nr);
    }

    void delete_case3(node_t* p, bool nr) {
        node_t* s = p->link[!nr];
        if (is_black(p) && is_black_blob(s)) {
            s->color = rb_red;
            delete_case1(p->parent, p == p->parent->link[1]);
        } else {
            delete_case4(p, nr);
        }
    }

    void delete_case4(node_t* p, bool nr) {
        node_t* s = p->link[!nr];
        if (is_red(p) && is_black_blob(s)) {
            s->color = rb_red;
            p->color = rb_black;
        } else {
            delete_case5(p, nr);
        }
    }

    void delete_case5(node_t* p, bool nr) {
        node_t* s = p->link[!nr];
        if (is_black(s)) {
            if (!nr && is_black(s->link[1])) {
                assert(is_red(s->link[0]));
                s->color = rb_red;
                s->link[0]->color = rb_black;
                right_rotate(s);
            } else if (nr && is_black(s->link[0])) {
                assert(is_red(s->link[1]));
                s->color = rb_red;
                s->link[1]->color = rb_black;
                left_rotate(s);
            }
        }
        delete_case6(p, nr);
    }

    void delete_case6(node_t* p, bool nr) {
        node_t* s = p->link[!nr];
        s->color = p->color;
        p->color = rb_black;
        if (!nr) {
            if (s->link[1])
                s->link[1]->color = rb_black;
            left_rotate(p);
        } else {
            if (s->link[0])
                s->link[0]->color = rb_black;
            right_rotate(p);
        }
    }

    /**
     *     p        p          p        p          p        p          p        p
     *     |        |          |        |          |        |          |        |
     *    (y)  ->   x          y   ->   x         (y)  ->              y   ->
     *    /                   /
     *   x                  (x)                                      rebalance at p
     *                                                               one less black
     */
    void erase_node_pull_left(node_t* y) {
        node_t* x = y->link[0];
        node_t* parent = y->parent;
        bool yp = y == parent->link[1];
        adopt_node(parent, x, yp);
        if (y->color == rb_black)
            rebalance_after_black_erase(parent, yp);
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
     */
    void erase_node_minimum(node_t* y) {
        node_t* x = node_t::minimum(y->link[1]);
        node_t* w = x->parent;
        node_t* parent = y->parent;
        bool yp = y == parent->link[1];
        adopt_node(parent, x, yp);
        adopt_node(w, x->link[1], y == w);
        adopt_node(x, y->link[0], 0);
        adopt_node(x, y->link[1], 1);
        std::swap(x->color, y->color);
        if (y->color == rb_black)
            rebalance_after_black_erase(y == w ? x : w, y == w);
    }
    void erase_node_and_rebalance(node_t* y) {
        if (!y->link[1])
            erase_node_pull_left(y);
        else
            erase_node_minimum(y);
    }

    /**
     * Insert node y into the tree as a child of parent on the given side.
     * parent must not have a child on that side and y must be a free node.
     *
     *    parent         parent                 parent        parent
     *     /       ->     /  \         or           \    ->    /  \
     *   [l]            [l]   y                     [r]       y   [r]
     */
    void insert_node(node_t* parent, node_t* y, bool is_right) {
        adopt_node(parent, y, is_right);
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
        erase_node_and_rebalance(y);
        drop_node(y);
        node_count--;
    }
    /**
     * Remove node y from the tree but do not destroy it.
     */
    void yank_node(node_t* y) {
        erase_node_and_rebalance(y);
        clear_node(y);
        node_count--;
    }

    void debug() const {
        RB_ASSERT(head && !head->link[1] && head->color == rb_red);
        size_t cnt = 0;
        debug_node(head->link[0], head, cnt);
        RB_ASSERT(cnt == node_count);
    }

  private:
    void print_tree_inorder() const {
        RB_DEBUG("===== count: %02d ===========\n", int(node_count));
        print_tree_inorder(head->link[0], "", false);
        RB_DEBUG("===========================\n");
    }

    void print_tree_inorder(node_t* n, std::string prefix, bool bar) const {
        if (!n) {
            RB_DEBUG("%s %c\n", prefix.data(), bar ? '|' : '+');
        } else {
            RB_DEBUG("%s +-- %s\n", prefix.data(), print_node(n).data());
            prefix += bar ? " |  " : "    ";
            if (n->link[0] || n->link[1]) {
                print_tree_inorder(n->link[0], prefix, true);
                print_tree_inorder(n->link[1], prefix, false);
            }
        }
    }

    static inline std::string print_node(node_t* node) noexcept {
        std::string s;
        s += std::to_string(node->data);
        if (node->color == rb_red)
            s += "(**)";
        if (node->parent != node->parent->parent)
            s += "   @(" + std::to_string(node->parent->data) + ")";
        if (node->link[0])
            s += "   <(" + std::to_string(node->link[0]->data) + ")";
        if (node->link[1])
            s += "   >(" + std::to_string(node->link[1]->data) + ")";
        return s;
    }

    int debug_node(const node_t* y, const node_t* parent, size_t& cnt) const {
        if (!y)
            return 0;
        cnt++;
        RB_ASSERT(y->parent == parent);
        RB_ASSERT(parent->color == rb_black || y->color == rb_black);
        int l = debug_node(y->link[0], y, cnt);
        int r = debug_node(y->link[1], y, cnt);
        RB_ASSERT(l == r);
        return l + y->color;
    }
};
