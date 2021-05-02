#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <bits/stdc++.h>

using namespace std;

template <typename Vec = string, typename T = typename Vec::value_type>
struct suffix_tree {
    static constexpr int A = 26;
    static constexpr int chash(T value) { return value - 'a'; }

    struct Node {}
};

#endif // SUFFIX_TREE_HPP
