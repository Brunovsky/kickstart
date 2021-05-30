#ifndef POLY_MULTIEVAL_HPP
#define POLY_MULTIEVAL_HPP

#include "poly.hpp"

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) struct multieval_tree {
    vector<array<int, 2>> ranges;
    vector<vector<T>> tree;
    vector<T> x;
};

tmpl(T) auto build_multieval_tree(const vector<T>& x) {
    int N = x.size(), M = 1 << fft::next_two(N);

    // We build a perfect segtree (no wasted nodes)
    // Build leaves, then align leaves, then pushup nodes

    vector<array<int, 2>> ranges(2 * N);
    vector<vector<T>> tree(2 * N);
    for (int i = 0; i < N; i++) {
        ranges[i + N] = {i, i + 1};
        tree[i + N] = {-x[i], 1};
    }

    rotate(begin(ranges) + N, begin(ranges) + (3 * N - M), end(ranges));
    rotate(begin(tree) + N, begin(tree) + (3 * N - M), end(tree));

    for (int i = N - 1; i >= 1; i--) {
        int l = i << 1, r = i << 1 | 1;
        ranges[i] = {ranges[l][0], ranges[r][1]};
        assert(ranges[i][0] + 1 < ranges[i][1]);
        tree[i] = tree[l] * tree[r];
        assert(int(tree[i].size()) == ranges[i][1] - ranges[i][0] + 1);
    }

    return multieval_tree<T>{move(ranges), move(tree), x};
}

tmpl(T) void multieval_dfs(int i, const vector<T>& b, vector<T>& value,
                           const multieval_tree<T>& evaltree) {
    const auto& [ranges, tree, x] = evaltree;
    auto [L, R] = ranges[i];
    if (L + 1 == R) {
        value[L] = eval(b, x[L]);
    } else {
        int l = i << 1, r = i << 1 | 1;
        multieval_dfs(l, b % tree[l], value, evaltree);
        multieval_dfs(r, b % tree[r], value, evaltree);
    }
}

tmpl(T) auto multieval(const vector<T>& a, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, a % evaltree.tree[1], value, evaltree);
    return value;
}

tmpl(T) auto multieval(const vector<T>& a, const vector<T>& x) {
    return multieval(a, build_multieval_tree(x));
}

#undef tmpl

} // namespace polymath

#endif // POLY_MULTIEVAL_HPP
