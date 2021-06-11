#pragma once

#include "poly.hpp"

/**
 * Multipoint evaluation and polynomial interpolation in O(N log^2(N))
 * No fast paths for small cases yet (breakeven around 3000)
 */

namespace polymath {

#define tmpl(T) template <typename T>

tmpl(T) struct multieval_tree {
    vector<int> index;
    vector<vector<T>> tree;
    vector<T> x;
};

tmpl(T) auto build_multieval_tree(const vector<T>& x) {
    int N = x.size(), M = 1 << fft::next_two(N);
    vector<int> index(N);
    vector<vector<T>> tree(2 * N);
    for (int i = 0; i < N; i++) {
        index[i] = i;
        tree[i + N] = {-x[i], 1};
    }

    rotate(begin(index), begin(index) + (2 * N - M), end(index));
    rotate(begin(tree) + N, begin(tree) + (3 * N - M), end(tree));

    for (int i = N - 1; i >= 1; i--) {
        int l = i << 1, r = i << 1 | 1;
        tree[i] = tree[l] * tree[r];
    }

    return multieval_tree<T>{move(index), move(tree), x};
}

tmpl(T) void multieval_dfs(int i, const vector<T>& poly, vector<T>& value,
                           const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        value[j] = eval(poly, x[j]);
    } else {
        int l = i << 1, r = i << 1 | 1;
        multieval_dfs(l, poly % tree[l], value, evaltree);
        multieval_dfs(r, poly % tree[r], value, evaltree);
    }
}

tmpl(T) auto multieval(const vector<T>& poly, const multieval_tree<T>& evaltree) {
    vector<T> value(evaltree.x.size());
    multieval_dfs(1, poly % evaltree.tree[1], value, evaltree);
    return value;
}

tmpl(T) auto multieval(const vector<T>& poly, const vector<T>& x) {
    return multieval(poly, build_multieval_tree(x));
}

tmpl(T) auto interpolate_dfs(int i, const vector<T>& poly, const vector<T>& y,
                             const multieval_tree<T>& evaltree) {
    const auto& [index, tree, x] = evaltree;
    if (int N = x.size(); i >= N) {
        int j = index[i - N];
        return vector<T>{y[j] / poly[0]};
    } else {
        int l = i << 1, r = i << 1 | 1;
        auto a = interpolate_dfs(l, poly % tree[l], y, evaltree);
        auto b = interpolate_dfs(r, poly % tree[r], y, evaltree);
        return a * tree[r] + b * tree[l];
    }
}

tmpl(T) auto interpolate(const vector<T>& x, const vector<T>& y) {
    assert(x.size() == y.size());
    auto evaltree = build_multieval_tree(x);
    return interpolate_dfs(1, deriv(evaltree.tree[1]), y, evaltree);
}

#undef tmpl

} // namespace polymath
