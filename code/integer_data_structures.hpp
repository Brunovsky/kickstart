#ifndef INTEGER_DATA_STRUCTURES_HPP
#define INTEGER_DATA_STRUCTURES_HPP

#include <cassert>
#include <cstdlib>
#include <list>
#include <numeric>
#include <vector>

using namespace std;

// *****

/**
 * Optimized versions of simple data structures like trees, queues and lists for special
 * case over the integers [0...M).
 * Parent/child/list relationships are kept in vectors instead of pointers.
 *
 * Ideas from: https://github.com/indy256/codelibrary
 */

/**
 * Numbered collection of forward linked lists over the integers [0...N).
 * Each list maintains a head pointer and each int maintains a next pointer.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next pointers are not reset by clear()
 */
struct forward_lists {
    int L, N;
    vector<int> head, next;

    // L: number of lists (lists [0...L)), N: number of integers (integers [0...N))
    explicit forward_lists(int L = 0, int N = 0) { assign(L, N); }

    // start a new list l with the integer n
    void init(int l, int n) { head[l] = n, next[n] = -1; }
    // copy list b to list l
    void copy(int l, int b) { head[l] = head[b]; }
    // set the start of list l to integer n
    void view(int l, int n) { head[l] = n; }
    // swap lists l and t
    void swap(int l, int t) { swap(head[l], head[t]); }
    // whether list l is empty
    bool empty(int l) const { return head[l] == -1; }
    // clear list l
    void clear(int l) { head[l] = -1; }

    // push integer n into the head of list l
    void push(int l, int n) { next[n] = head[l], head[l] = n; }

    // clear all lists
    void clear() { fill(begin(head), end(head), -1); }

    // reset the structure
    void assign(int L, int N) {
        this->L = L, this->N = N;
        head.assign(L, -1), next.assign(N, -1);
    }
};

#define FOR_EACH_IN_FORWARD_LIST(i, l, lists) \
    for (int i = lists.head[l]; i != -1; i = lists.next[i])

/**
 * Numbered collection of doubly linked lists over the integers [0...N)
 * Each list maintains head/tail pointers and each int maintains next/prev pointers.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next/prev pointers (for integers) are not reset by clear()
 */
struct linked_lists {
    int L, N;
    vector<int> next, prev;

    // L: number of lists (lists [0...L)), N: number of integers (integers [0...N))
    explicit linked_lists(int L = 0, int N = 0) { assign(L, N); }

    // start a new list l with the integer n.
    void init(int l, int n) { meet(rep(l), n, rep(l)); }
    // head of a list l (first integer)
    int head(int l) const { return next[rep(l)]; }
    // tail of a list l (last integer)
    int tail(int l) const { return prev[rep(l)]; }
    // representative of a list l.
    int rep(int l) const { return l + N; }
    // whether list l is empty
    bool empty(int l) const { return next[rep(l)] == rep(l); }
    // clear list l
    void clear(int l) { next[rep(l)] = prev[rep(l)] = rep(l); }
    // swap lists l and t
    void swap(int l, int t) {
        std::swap(next[rep(l)], next[rep(t)]);
        std::swap(prev[rep(l)], prev[rep(t)]);
    }

    // insert integer n into the front of list l.
    void push_front(int l, int n) { meet(rep(l), n, head(l)); }
    // insert integer n into the back of list l.
    void push_back(int l, int n) { meet(tail(l), n, rep(l)); }
    // insert integer n before integer i into i's list.
    void insert_before(int i, int n) { meet(prev[i], n, i); }
    // insert integer n after integer i into i's list.
    void insert_after(int i, int n) { meet(i, n, next[i]); }
    // erase integer n from its list.
    void erase(int n) { meet(prev[n], next[n]); }

    // splice list b into the front of list l, and clear list b.
    void splice_front(int l, int b) { assert(l != b), splice_before(head(l), b); }
    // splice list b into the back of list l, and clear list b.
    void splice_back(int l, int b) { assert(l != b), splice_after(tail(l), b); }
    // splice list b before integer i into i's list, and clear list b. i must have a prev.
    void splice_before(int i, int b) {
        if (!empty(b))
            meet(prev[i], head(b)), meet(tail(b), i), clear(b);
    }
    // splice list b after integer i into i's list, and clear list b. i must have a next.
    void splice_after(int i, int b) {
        if (!empty(b))
            meet(tail(b), next[i]), meet(i, head(b)), clear(b);
    }

    // clear all lists
    void clear() {
        iota(begin(next) + N, end(next), N);
        iota(begin(prev) + N, end(prev), N);
    }

    // reset the structure
    void assign(int L, int N) {
        this->L = L, this->N = N;
        next.resize(N + L), prev.resize(N + L);
        clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

/**
 * Linked lists augmented with a list of unused list numbers, so that an unused list can
 * be requested when needed and released when used.
 */
template <typename lists>
struct ondemand_lists : lists {
    int h = 0;
    vector<int> que;

    // L: number of lists (lists [0...L)), N: number of integers (integers [0...N))
    explicit ondemand_lists(int L = 0, int N = 0) { assign(L, N); }

    // request a new unused list number
    int demand() { return give_one(); }
    // clear list l and release it
    void clear(int l) { lists::clear(l), take_one(l); }
    // clear all lists
    void clear() { lists::clear(), take_all(); }
    // reset the structure
    void assign(int L, int N) { lists::assign(L, N), que.resize(L), take_all(); }

  private:
    int give_one() {
        assert(h != lists::L);
        int u = h;
        h = que[h], que[u] = -1;
        return u;
    }
    void take_one(int l) {
        if (que[l] == -1) {
            que[l] = h, h = l;
        }
    }
    void take_all() { h = 0, iota(begin(que), end(que), 1); }
};

#define FOR_EACH_IN_LINKED_LIST(i, l, lists) \
    for (int i = lists.head(l); i != lists.rep(l); i = lists.next[i])

#define FOR_EACH_IN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int i = lists.tail(l); i != lists.rep(l); i = lists.prev[i])

/**
 * Forest of rooted trees over the integers [0...N)
 * Each node maintains a parent pointer and next/prev pointers for its siblings. Each
 * node may or may not belong to the forest. The forest is implicitly rooted on the
 * phantom node N. Roots of the forest are the childs of N. Each node n has a
 * representative rep(n) which is used to maintain the next/prev pointers, just like
 * in linked_lists. With appropriate usage the trees are kept acyclic and disjoint,
 * but the usage can be more exotic.
 */
struct int_forest {
    int N, S;
    vector<int> parent, next, prev;

    explicit int_forest(int N = 0, bool empty_forest = 1) { assign(N, empty_forest); }

    // representative of a node n.
    int rep(int n) const { return n + N + 1; }
    // head of the sibling list of node n (first element)
    int head(int n) const { return next[rep(n)]; }
    // tail of the sibling list of node n (last element)
    int tail(int n) const { return prev[rep(n)]; }
    // head and tail of sibling list of node n
    pair<int, int> sibl(int n) const { return {head(n), tail(n)}; }
    // next sibling of n.
    int next_sibling(int n) const { return next[n]; }
    // prev sibling of n.
    int prev_sibling(int n) const { return prev[n]; }
    // whether node n is in the forest.
    bool contains(int n) const { return parent[n] != -1; }
    // whether the tree rooted at n has no children.
    bool empty(int n) const { return head(n) > N; }
    // whether the forest has no trees.
    bool empty() const { return empty(N); }
    // whether n is a representative node.
    bool is_rep(int n) const { return n > N; }

    // place n in the forest as a unit tree (n must not be in the forest)
    void place(int n) { assert(parent[n] == -1), splice(N, n); }
    // drop n from the forest (n must not have any children)
    void drop(int n) { assert(empty(n)), meet(prev[n], next[n]), parent[n] = -1; }

    // splice n's tree and place it after node s, so that n and s becomes siblings.
    void splice_after(int s, int n) {
        assert(n < N && s <= N && parent[s] != -1);
        meet(prev[n], next[n]), meet(s, n, next[s]), parent[n] = parent[s];
    }
    // splice n's tree and place it after node s, so that n and s becomes siblings.
    void splice_before(int s, int n) {
        assert(n < N && s <= N && parent[s] != -1);
        meet(prev[n], next[n]), meet(prev[s], n, s), parent[n] = parent[s];
    }
    // splice n's tree and set n's parent to p, anywhere in p's child list.
    void splice(int p, int n) {
        assert(n < N && p <= N && n != p && p != -1 && parent[n] != p);
        meet(prev[n], next[n]), meet(tail(p), n, rep(p)), parent[n] = p;
    }

    // swap n and m tree positions. careful of cycles.
    void swap_places(int n, int m) {
        assert(n < N && m < N && n != m);
        swap(parent[n], parent[m]), permute(n, m);
    }

    // swap n with its parent in O(#siblings(n)) time
    void promote(int n) {
        assert(n < N && parent[n] != -1 && parent[n] < N && !empty(parent[n]));
        int p = parent[n];
        int rn = rep(n), hn = head(n), tn = tail(n);
        int rp = rep(p), hp = head(p), tp = tail(p);
        parent[n] = parent[p], permute(n, p);
        if (empty(n)) {
            meet(rn, hp), meet(tp, rn);
            next[rp] = prev[rp] = rp;
        } else {
            meet(rp, hn), meet(tn, rp);
            meet(rn, hp), meet(tp, rn);
        }
        for (int i = head(n); i != rep(n); i = next[i])
            parent[i] = n;
    }

    // reset the forest to either an empty forest or N unit trees
    void clear(bool empty_forest = 1) {
        int R = N + 1, P = empty_forest ? -1 : N;
        auto x = parent.data(), n = next.data(), p = prev.data();
        fill(x + 0, x + N, P); // parent[n] = N or -1
        iota(n + 0, n + R, R); // next[n] = rep(n)
        iota(n + R, n + S, R); // next[rep(n)] = rep(n)
        iota(p + 0, p + R, R); // prev[n] = rep(n)
        iota(p + R, p + S, R); // prev[rep(n)] = rep(n)
        x[N] = N;
    }

    // reset the structure, updating N
    void assign(int N, bool empty_forest = 1) {
        this->N = N, this->S = 2 * N + 2;
        parent.resize(N + 1), next.resize(S), prev.resize(S);
        clear(empty_forest);
    }

  private:
    inline void meet(int n, int m) { next[n] = m, prev[m] = n; }
    inline void meet(int n, int m, int k) { meet(n, m), meet(m, k); }
    inline void permute(int n, int m) {
        int a = prev[n], b = next[n], c = prev[m], d = next[m];
        if (b == m) // a n m d --> a m n d
            meet(a, m, n), meet(m, n, d);
        else if (a == m) // c m n b --> c n m b
            meet(c, n, m), meet(n, m, b);
        else // a n b .. c m d --> a m b .. c n d
            meet(a, m, b), meet(c, n, d);
    }
};

#define FOR_EACH_CHILD_INT_TREE(i, n, forest) \
    for (int i = forest.head(n); !forest.is_rep(i); i = forest.next[i])

#define FOR_EACH_CHILD_INT_TREE_REVERSE(i, n, forest) \
    for (int i = forest.tail(n); !forest.is_rep(i); i = forest.prev[i])

#define FOR_EACH_NEXT_SIBLING_INT_TREE(i, s, forest) \
    for (int i = forest.next_sibling(s); !forest.is_rep(i); i = forest.next[i])

#define FOR_EACH_PREV_SIBLING_INT_TREE(i, s, forest) \
    for (int i = forest.prev_sibling(s); !forest.is_rep(i); i = forest.prev[i])

#define FOR_EACH_ANCESTOR_INT_TREE(i, n, forest) \
    for (int i = forest.parent[n]; i != forest.N; i = forest.parent[i])

/**
 * Binary heap over the integers [0...N) with decrease-key.
 * By default a min-heap, but you'll usually need a custom compare (e.g. dijkstra).
 */
template <typename Compare = less<>>
struct binary_heap {
    vector<int> c, idx;
    Compare comp;

    explicit binary_heap(int N = 0, const Compare& comp = Compare())
        : c(0, 0), idx(N, -1), comp(comp) {}

    bool empty() const { return c.empty(); }
    uint size() const { return c.size(); }
    bool contains(int n) const { return idx[n] != -1; }
    int top() const { return assert(!empty()), c[0]; }
    void clear() {
        for (int n : c)
            idx[n] = -1;
        c.clear();
    }

    void push(int n) {
        assert(!contains(n));
        idx[n] = c.size(), c.push_back(n);
        heapify_up(idx[n]);
    }

    int pop() {
        assert(!empty());
        int n = c[0];
        swap(c[0], c.back());
        idx[c[0]] = 0, idx[n] = -1;
        c.pop_back();
        heapify_down(0);
        return n;
    }

    void improve(int n) { assert(contains(n)), heapify_up(idx[n]); }
    void decline(int n) { assert(contains(n)), heapify_down(idx[n]); }
    void push_or_improve(int n) { contains(n) ? improve(n) : push(n); }
    void push_or_decline(int n) { contains(n) ? decline(n) : push(n); }

  private:
    void heapify_up(int i) {
        while (i > 0 && comp(c[i], c[(i - 1) >> 1])) // while c[i] < c[parent(i)]
            exchange(i, (i - 1) >> 1), i = (i - 1) >> 1;
    }

    void heapify_down(int i) {
        int k, S = c.size();
        while ((k = i << 1 | 1) < S) {
            if (k + 1 < S && !comp(c[k], c[k + 1])) // if c[rchild(i)] <= c[lchild(i)]
                k++;
            if (!comp(c[k], c[i])) // break if c[i] <= c[minchild(i)]
                break;
            exchange(i, k), i = k;
        }
    }

    void exchange(int i, int j) { swap(idx[c[i]], idx[c[j]]), swap(c[i], c[j]); }
};

#endif // INTEGER_DATA_STRUCTURES_HPP
