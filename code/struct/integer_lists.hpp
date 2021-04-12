#ifndef INTEGER_LISTS_HPP
#define INTEGER_LISTS_HPP

#include <bits/stdc++.h>

using namespace std;

// *****

/**
 * Ideas from: https://github.com/indy256/codelibrary
 */

/**
 * 2d dancing links, non-circular
 */
struct dancing_links_matrix {
    int n = 0, m = 0;
    vector<vector<int>> S, N, E, W;

    dancing_links_matrix() = default;
    dancing_links_matrix(int n, int m) { init(n, m); }

    void init(int n, int m) {
        this->n = n, this->m = m;
        N.assign(n + 2, vector<int>(m + 2));
        S.assign(n + 2, vector<int>(m + 2));
        E.assign(n + 2, vector<int>(m + 2));
        W.assign(n + 2, vector<int>(m + 2));
        for (int i = 0; i <= n + 1; i++) {
            for (int j = 0; j <= m + 1; j++) {
                S[i][j] = i + 1;
                N[i][j] = i - 1;
                E[i][j] = j + 1;
                W[i][j] = j - 1;
            }
        }
    }
    using int2 = array<int, 2>;
    void insert(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = c;
        W[r][E[r][c]] = c;
        S[N[r][c]][c] = r;
        N[S[r][c]][c] = r;
    }
    void remove(int2 p) {
        int r = p[0], c = p[1];
        E[r][W[r][c]] = E[r][c];
        W[r][E[r][c]] = W[r][c];
        S[N[r][c]][c] = S[r][c];
        N[S[r][c]][c] = N[r][c];
    }
    int2 south(int2 p) const {
        int r = p[0], c = p[1];
        return {S[r][c], c};
    }
    int2 north(int2 p) const {
        int r = p[0], c = p[1];
        return {N[r][c], c};
    }
    int2 east(int2 p) const {
        int r = p[0], c = p[1];
        return {r, E[r][c]};
    }
    int2 west(int2 p) const {
        int r = p[0], c = p[1];
        return {r, W[r][c]};
    }
};

/**
 * Dynamic queue over the integers [0...M), allowing ints to be released from anywhere
 * in the queue.
 */
struct int_freelist {
    int N, head = 0;
    vector<int> next;

    // N: number of integers (integers[0...N))
    explicit int_freelist(int N = 0) { assign(N); }

    bool empty() const { return head == N; }
    bool taken(int n) const { return next[n] != -1; }
    int request(int n = 0) { return n = head, head = next[n], next[n] = -1, n; }
    void release(int n) { assert(next[n] == -1), next[n] = head, head = n; }
    void clear() { head = N, fill(begin(next), end(next), -1); }
    void reset() { head = 0, iota(begin(next), end(next), 1); }
    void assign(int N) { this->N = N, next.resize(N), reset(); }
};

/**
 * Numbered collection of forward linked lists over the integers [0...N).
 * Each list maintains a head pointer and each int maintains a next pointer.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next pointers are not reset by clear()
 * Usage examples:
 *     bucket sorting
 *     list of stacks with LIFO semantics
 *       fl.push(s, n)                     -- add value n to set s
 *       n = fl.head(s)                    -- read the top of the set s
 *       fl.pop(s)                         -- pop the top of the set s
 *       fl.empty(s)                       -- check whether set s is empty
 *       FOR_EACH_IN_FORWARD_LIST(i,s,fl)  -- iterate the elements of the set
 *       fl.clear(s)                       -- clear the set s after iterating it
 */
struct forward_lists {
    int L, N;
    vector<int> next;

    // L: lists are [0...L), N: integers are [0...N)
    explicit forward_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    bool empty(int l) const { return head(l) == -1; }

    void init(int l, int n) { next[rep(l)] = n, next[n] = -1; }
    void clear(int l) { next[rep(l)] = -1; }
    void exchange(int l, int t) { swap(next[rep(l)], next[rep(t)]); }

    void push(int l, int n) { insert(rep(l), n); }
    void insert(int i, int n) { next[n] = next[i], next[i] = n; }
    void pop(int l) { assert(head(l) != -1), next[rep(l)] = next[head(l)]; }

    void clear() { fill(begin(next) + N, end(next), -1); }
    void assign(int L, int N) { this->L = L, this->N = N, next.assign(L + N, -1); }
};

#define FOR_EACH_IN_FORWARD_LIST(i, l, lists) \
    for (int i = lists.head(l); i != -1; i = lists.next[i])

/**
 * Numbered collection of doubly linked lists over the integers [0...N)
 * Each list maintains head/tail pointers and each int maintains next/prev pointers.
 * With appropriate usage the lists are kept acyclic and disjoint, but the usage can
 * be more exotic.
 * Caution: next/prev pointers (for integers) are not reset by clear()
 *     bucket sorting
 *     list of stacks with FIFO semantics
 *     list of sets with positional insert and erase
 * Usage:
 *       ll.init(s, n)                     -- initialize set s by adding free value n
 *       ll.push_back(s, n)                -- add free value n to set s
 *       n = ll.head(s)                    -- read the front of the set s
 *       n = ll.tail(s)                    -- read the back of the set s
 *       ll.pop_front(s)                   -- pop the top of the set s
 *       ll.insert_after(i, n)             -- add free value n after i in i's set
 *       ll.insert_before(i, n)            -- add free value n before i in i's set
 *       ll.splice_front(l, b)             -- put set b at the front of set l and clear b
 *       ll.splice_back(l, b)              -- put set b at the back of set l and clear b
 *       ll.empty(s)                       -- check whether set s is empty
 *       ll.clear(s)                       -- clear the set s after iterating it
 *       FOR_EACH_IN_LINKED_LIST(i,s,ll)   -- iterate forward the elements of the set s
 */
struct linked_lists {
    int L, N;
    vector<int> next, prev;

    // L: lists are [0...L), N: integers are [0...N)
    explicit linked_lists(int L = 0, int N = 0) { assign(L, N); }

    int rep(int l) const { return l + N; }
    int head(int l) const { return next[rep(l)]; }
    int tail(int l) const { return prev[rep(l)]; }
    bool empty(int l) const { return next[rep(l)] == rep(l); }

    void init(int l, int n) { meet(rep(l), n, rep(l)); }
    void clear(int l) { next[rep(l)] = prev[rep(l)] = rep(l); }
    void exchange(int l, int t) {
        assert(!empty(l) && !empty(t));
        int a = next[rep(l)], b = next[rep(t)], c = prev[rep(l)], d = prev[rep(t)];
        meet(rep(t), a), meet(rep(l), b), meet(c, rep(t)), meet(d, rep(l));
    }

    void push_front(int l, int n) { assert(l < L && n < N), meet(rep(l), n, head(l)); }
    void push_back(int l, int n) { assert(l < L && n < N), meet(tail(l), n, rep(l)); }
    void insert_before(int i, int n) { assert(i < N && n < N), meet(prev[i], n, i); }
    void insert_after(int i, int n) { assert(i < N && n < N), meet(i, n, next[i]); }
    void erase(int n) { assert(n < N), meet(prev[n], next[n]); }
    void pop_front(int l) { assert(l < L), meet(rep(l), next[head(l)]); }
    void pop_back(int l) { assert(l < L), meet(prev[tail(l)], rep(l)); }

    void splice_front(int l, int b) { assert(l != b), splice_before(head(l), b); }
    void splice_back(int l, int b) { assert(l != b), splice_after(tail(l), b); }
    void splice_before(int i, int b) {
        if (!empty(b))
            meet(prev[i], head(b)), meet(tail(b), i), clear(b);
    }
    void splice_after(int i, int b) {
        if (!empty(b))
            meet(tail(b), next[i]), meet(i, head(b)), clear(b);
    }

    void clear() {
        iota(begin(next) + N, end(next), N), iota(begin(prev) + N, end(prev), N);
    }
    void assign(int L, int N) {
        this->L = L, this->N = N, next.resize(N + L), prev.resize(N + L), clear();
    }

  private:
    inline void meet(int u, int v) { next[u] = v, prev[v] = u; }
    inline void meet(int u, int v, int w) { meet(u, v), meet(v, w); }
};

#define FOR_EACH_IN_LINKED_LIST(i, l, lists) \
    for (int z##i = l, i = lists.head(z##i); i != lists.rep(z##i); i = lists.next[i])

#define FOR_EACH_IN_LINKED_LIST_REVERSE(i, l, lists) \
    for (int z##i = l, i = lists.tail(z##i); i != lists.rep(z##i); i = lists.prev[i])

#endif // INTEGER_LISTS_HPP
