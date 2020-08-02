#include "avl_tree.hpp"

/**
 * STL-friendly tree template for (multi)sets and (multi)maps.
 *   - Parametrized by type T and comparison functor.
 *   - Does not use allocators (for simplicity).
 *
 * Uses the avl_tree.hpp core in avl_tree.hpp
 *
 * Done:
 *   - Tree core
 *   - Single and range insert and erase
 *   - All iterators
 *   - Support comparison operators
 *   - Support equal_range(), lower_bound() and upper_bound()
 *   - Support emplace(), emplace_hint(), and insert_hint()
 *   - Support merge() and extract()
 *   - Support inserters (avl_inserter_*)
 *   - Copying and moving
 *
 * TODO:
 *   - Define set, multiset wrappers
 *   - Define map, multimap wrappers
 *   - Red black core (?)
 */

// Tree core
template <typename T>
using Tree = avl_tree<T>;

template <typename BSTree>
struct bst_iterator;

template <typename BSTree>
struct bst_const_iterator;

template <typename BSTree>
struct bst_iterator {
  private:
    friend BSTree;
    friend bst_const_iterator<BSTree>;
    using T = typename BSTree::value_type;
    using node_t = typename BSTree::node_t;
    using self_type = bst_iterator<BSTree>;
    node_t* y;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = ptrdiff_t;

    bst_iterator() : y(nullptr) {}
    explicit bst_iterator(node_t* y) : y(y) {}

    explicit operator bool() const noexcept {
        return y != nullptr;
    }
    T& operator*() const noexcept {
        return y->data;
    }
    T* operator->() const noexcept {
        return y->data;
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

template <typename BSTree>
struct bst_const_iterator {
  private:
    friend BSTree;
    friend bst_iterator<BSTree>;
    using T = typename BSTree::value_type;
    using node_t = typename BSTree::node_t;
    using self_type = bst_const_iterator<BSTree>;
    const node_t* y;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using difference_type = ptrdiff_t;

    bst_const_iterator() : y(nullptr) {}
    explicit bst_const_iterator(const node_t* y) : y(y) {}
    bst_const_iterator(bst_iterator<BSTree> it) : y(it.y) {}

    explicit operator bool() const noexcept {
        return y != nullptr;
    }
    const T& operator*() const noexcept {
        return y->data;
    }
    const T* operator->() const noexcept {
        return &y->data;
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

template <typename BSTree>
struct bst_node_handle {
  private:
    friend BSTree;
    using T = typename BSTree::value_type;
    using node_t = typename BSTree::node_t;
    using self_type = bst_node_handle<BSTree>;
    node_t* y;

  public:
    using key_type = typename BSTree::key_type;
    using value_type = typename BSTree::value_type;

    bst_node_handle() : y(nullptr) {}
    explicit bst_node_handle(node_t* y) : y(y) {}

    bst_node_handle(self_type&& other) : y(other.y) {
        other.y = nullptr;
    }
    bst_node_handle& operator=(self_type&& other) {
        delete y;
        y = other.y;
        other.y = nullptr;
    }

    ~bst_node_handle() {
        delete y;
    }

    bool empty() const noexcept {
        return y == nullptr;
    }
    explicit operator bool() const noexcept {
        return y != nullptr;
    }
    value_type& value() const {
        return y->data;
    }
    key_type& key() const {
        return y->data.first; // TODO: will this need const_cast?
    }
    void swap(self_type& other) noexcept {
        swap(y, other.y);
    }
    friend void swap(self_type& lhs, self_type& rhs) noexcept {
        swap(lhs.y, rhs.y);
    }
};

template <typename BSTree>
struct bst_insert_return_type {
    using iterator = typename BSTree::iterator;
    using node_type = typename BSTree::node_type;

    iterator position;
    bool inserted;
    node_type node;
};

template <typename T, typename Compare = std::less<T>>
struct bs_tree : protected Tree<T> {
  private:
    using tree_t = Tree<T>;
    using node_t = typename tree_t::node_t;
    using handle_t = bst_node_handle<bs_tree>;

    using tree_t::drop_node;
    using tree_t::erase_node;
    using tree_t::head;
    using tree_t::insert_node;
    using tree_t::insert_node_after;
    using tree_t::insert_node_before;
    using tree_t::maximum;
    using tree_t::minimum;
    using tree_t::yank_node;

  protected:
    Compare comp;

    template <typename K1 = T, typename K2 = T>
    inline bool compare(const K1& lhs, const K2& rhs) const noexcept {
        return comp(lhs, rhs);
    }

  public:
    using iterator = bst_iterator<bs_tree>;
    using const_iterator = bst_const_iterator<bs_tree>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using node_type = bst_node_handle<bs_tree>;
    using insert_return_type = bst_insert_return_type<bs_tree>;
    friend iterator;
    friend const_iterator;
    friend node_type;

    using key_type = T;
    using value_type = T;
    using size_type = typename Tree<T>::size_type;
    using key_compare = Compare;
    using value_compare = Compare;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    bs_tree() : tree_t() {}

    explicit bs_tree(const Compare& comp) : comp(comp) {}

    bs_tree(const bs_tree& other) = default;
    bs_tree(bs_tree&& other) = default;
    bs_tree& operator=(const bs_tree& other) = default;
    bs_tree& operator=(bs_tree&& other) = default;

    using tree_t::clear;
    using tree_t::debug;
    using tree_t::empty;
    using tree_t::max_size;
    using tree_t::size;

    void swap(bs_tree& other) {
        using std::swap;
        tree_t::swap(other);
        swap(comp, other.comp);
    }
    friend inline void swap(bs_tree& lhs, bs_tree& rhs) noexcept {
        lhs.swap(rhs);
    }

    inline iterator begin() noexcept {
        return iterator(tree_t::minimum());
    }
    inline iterator end() noexcept {
        return iterator(head);
    }
    inline const_iterator begin() const noexcept {
        return const_iterator(tree_t::minimum());
    }
    inline const_iterator end() const noexcept {
        return const_iterator(head);
    }
    inline const_iterator cbegin() const noexcept {
        return const_iterator(tree_t::minimum());
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
    template <typename K = T>
    node_t* find_node(const K& key) {
        node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(key, x->data);
            if (!lesser && !compare(x->data, key))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = T>
    const node_t* find_node(const K& key) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(key, x->data);
            if (!lesser && !compare(x->data, key))
                return x;
            x = x->link[!lesser];
        }
        return head;
    }
    template <typename K = T>
    node_t* lower_bound_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (!compare(x->data, key))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = T>
    const node_t* lower_bound_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (!compare(x->data, key))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = T>
    node_t* upper_bound_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (compare(key, x->data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = T>
    const node_t* upper_bound_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (compare(key, x->data))
                y = x, x = x->link[0];
            else
                x = x->link[1];
        }
        return y;
    }
    template <typename K = T>
    std::pair<node_t*, node_t*> equal_range_node(const K& key) {
        node_t* x = head->link[0];
        node_t* y = head;
        while (x) {
            if (compare(x->data, key))
                x = x->link[1];
            else if (compare(key, x->data))
                y = x, x = x->link[0];
            else {
                node_t* xu = x->link[1];
                node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!compare(x->data, key))
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
    template <typename K = T>
    std::pair<const node_t*, const node_t*> equal_range_node(const K& key) const {
        const node_t* x = head->link[0];
        const node_t* y = head;
        while (x) {
            if (compare(x->data, key))
                x = x->link[1];
            else if (compare(key, x->data))
                y = x, x = x->link[0];
            else {
                const node_t* xu = x->link[1];
                const node_t* yu = y;
                y = x, x = x->link[0];
                // lower bound [x, y]
                while (x) {
                    if (!compare(x->data, key))
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
    template <typename K = T>
    iterator find(const K& key) {
        return iterator(find_node(key));
    }
    template <typename K = T>
    const_iterator find(const K& key) const {
        return const_iterator(find_node(key));
    }
    template <typename K = T>
    iterator lower_bound(const K& key) {
        return iterator(lower_bound_node(key));
    }
    template <typename K = T>
    const_iterator lower_bound(const K& key) const {
        return const_iterator(lower_bound_node(key));
    }
    template <typename K = T>
    iterator upper_bound(const K& key) {
        return iterator(upper_bound_node(key));
    }
    template <typename K = T>
    const_iterator upper_bound(const K& key) const {
        return const_iterator(upper_bound_node(key));
    }
    template <typename K = T>
    std::pair<iterator, iterator> equal_range(const K& key) {
        auto nodes = equal_range_node(key);
        return {iterator(nodes.first), iterator(nodes.second)};
    }
    template <typename K = T>
    std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
        auto nodes = equal_range_node(key);
        return {const_iterator(nodes.first), const_iterator(nodes.second)};
    }
    template <typename K = T>
    bool contains(const K& data) const {
        const node_t* x = head->link[0];
        while (x) {
            bool lesser = compare(data, x->data);
            if (!lesser && !compare(x->data, data))
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

  private:
    std::pair<iterator, bool> try_insert_node_unique(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = compare(node->data, y->data);
            if (!lesser && !compare(y->data, node->data))
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
            drop_node(node);
        return res;
    }
    std::pair<iterator, bool> try_insert_node_hint_unique(node_t* node, node_t* hint) {
        if (hint == head) {
            if (size() > 0 && compare(tree_t::maximum()->data, node->data)) {
                insert_node(tree_t::maximum(), node, 1);
                return {iterator(node), true};
            }
            return try_insert_node_unique(node); // bad hint
        } else if (compare(node->data, hint->data)) {
            if (hint == tree_t::minimum()) {
                insert_node(tree_t::minimum(), node, 0);
                return {iterator(node), true};
            }
            node_t* prev = node_t::decrement(hint);
            if (compare(prev->data, node->data)) {
                insert_node_before(hint, node);
                return {iterator(node), true};
            }
            return try_insert_node_unique(node); // bad hint
        } else if (compare(hint->data, node->data)) {
            return try_insert_node_unique(node); // bad hint
        } else {
            return {iterator(hint), false};
        }
    }
    std::pair<iterator, bool> insert_node_hint_unique(node_t* node, node_t* hint) {
        auto res = try_insert_node_hint_unique(node, hint);
        if (!res.second)
            drop_node(node);
        return res;
    }
    iterator insert_node_multi(node_t* node) {
        node_t* y = head->link[0];
        node_t* parent = head;
        bool lesser = true;
        while (y) {
            lesser = compare(node->data, y->data);
            parent = y;
            y = y->link[!lesser];
        }
        insert_node(parent, node, !lesser);
        return iterator(node);
    }
    iterator insert_node_hint_multi(node_t* node, node_t* hint) {
        if (hint == head) {
            if (size() > 0 && compare(tree_t::maximum()->data, node->data)) {
                insert_node(tree_t::maximum(), node, 1);
                return iterator(node);
            }
            return insert_node_multi(node); // bad hint
        } else if (compare(node->data, hint->data)) {
            if (hint == tree_t::minimum()) {
                insert_node(tree_t::minimum(), node, 0);
                return iterator(node);
            }
            node_t* prev = node_t::decrement(hint);
            if (compare(prev->data, node->data)) {
                insert_node_before(hint, node);
                return iterator(node);
            }
            return insert_node_multi(node);
        } else if (compare(hint->data, node->data)) {
            return insert_node_multi(node);
        } else {
            insert_node_before(hint, node);
            return iterator(node);
        }
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
        node_t* node = new node_t(data);
        return insert_node_unique(node);
    }
    iterator insert_hint_unique(const_iterator hint, const T& data) {
        node_t* node = new node_t(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node).first;
    }
    std::pair<iterator, bool> insert_unique(T&& data) {
        node_t* node = new node_t(std::move(data));
        return insert_node_unique(node);
    }
    iterator insert_hint_unique(const_iterator hint, T&& data) {
        node_t* node = new node_t(std::move(data));
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
        node_t* node = new node_t(data);
        return insert_node_multi(node);
    }
    iterator insert_hint_multi(const_iterator hint, const T& data) {
        node_t* node = new node_t(data);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_multi(node, hint_node);
    }
    iterator insert_multi(T&& data) {
        node_t* node = new node_t(std::move(data));
        return insert_node_multi(node);
    }
    iterator insert_hint_multi(const_iterator hint, T&& data) {
        node_t* node = new node_t(std::move(data));
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
        node_t* node = new node_t(std::forward<Args>(args)...);
        return insert_node_unique(node);
    }
    template <typename... Args>
    iterator emplace_hint_unique(const_iterator hint, Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        node_t* hint_node = const_cast<node_t*>(hint.y);
        return insert_node_hint_unique(node, hint_node).first;
    }

    template <typename... Args>
    iterator emplace_multi(Args&&... args) {
        node_t* node = new node_t(std::forward<Args>(args)...);
        return insert_node_multi(node);
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
    size_t erase_multi(const T& data) {
        auto range = equal_range(data);
        size_t s = size();
        for (auto it = range.first; it != range.second;) {
            node_t* y = it.y;
            ++it;
            erase_node(y);
        }
        return size() - s;
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
    size_type erase_if(bs_tree& bst, Pred pred) {
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
        node_t* y = const_cast<node_t*>(pos.y);
        yank_node(y);
        return node_type(y);
    }
    node_type extract(const T& key) {
        node_t* y = find_node(key);
        if (y != head) {
            yank_node(y);
            return node_type(y);
        }
        return node_type();
    }

    template <typename CmpFn2>
    void merge_unique(bs_tree<T, CmpFn2>& src) {
        for (auto it = src.begin(); it != src.end();) {
            node_t* node = it.y;
            ++it;
            node_t* x = head->link[0];
            node_t* y = head;
            bool lesser = true;
            while (x) {
                lesser = compare(node->data, x->data);
                if (!lesser && !compare(x->data, node->data))
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
    void merge_unique(bs_tree<T, CmpFn2>&& src) {
        merge_unique(src);
    }
    template <typename CmpFn2>
    void merge_multi(bs_tree<T, CmpFn2>& src) {
        for (auto it = src.begin(); it != src.end();) {
            node_t* node = it.y;
            ++it;
            assert(node && node->parent);
            node_t* x = head->link[0];
            node_t* y = head;
            bool lesser = true;
            while (x) {
                lesser = compare(node->data, x->data);
                y = x;
                x = x->link[!lesser];
            }
            src.yank_node(node);
            insert_node(y, node, !lesser);
        }
    }
    template <typename CmpFn2>
    void merge_multi(bs_tree<T, CmpFn2>&& src) {
        merge_multi(src);
    }
};

template <typename T, typename Compare>
struct bst_inserter_unique_iterator {
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using self_type = bst_inserter_unique_iterator<T, Compare>;
    using iterator_category = std::output_iterator_tag;
    using container_type = bs_tree<T, Compare>;

    bst_inserter_unique_iterator(container_type& tree) : tree(&tree) {}

    self_type& operator*() {
        return *this;
    }
    self_type& operator++() {
        return *this;
    }
    self_type& operator++(int) {
        return *this;
    }
    self_type& operator=(const T& value) {
        tree->insert_unique(value);
        return *this;
    }
    self_type& operator=(T&& value) {
        tree->insert_unique(std::move(value));
        return *this;
    }

  private:
    container_type* tree;
};

template <typename T, typename Compare>
struct bst_inserter_multi_iterator {
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using self_type = bst_inserter_multi_iterator<T, Compare>;
    using iterator_category = std::output_iterator_tag;
    using container_type = bs_tree<T, Compare>;

    bst_inserter_multi_iterator(container_type& tree) : tree(&tree) {}

    self_type& operator*() {
        return *this;
    }
    self_type& operator++() {
        return *this;
    }
    self_type& operator++(int) {
        return *this;
    }
    self_type& operator=(const T& value) {
        tree->insert_multi(value);
        return *this;
    }
    self_type& operator=(T&& value) {
        tree->insert_multi(std::move(value));
        return *this;
    }

  private:
    container_type* tree;
};

template <typename T, typename Compare>
bst_inserter_unique_iterator<T, Compare> bst_inserter_unique(bs_tree<T, Compare>& tree) {
    return bst_inserter_unique_iterator<T, Compare>(tree);
}

template <typename T, typename Compare>
bst_inserter_multi_iterator<T, Compare> bst_inserter_multi(bs_tree<T, Compare>& tree) {
    return bst_inserter_multi_iterator<T, Compare>(tree);
}
