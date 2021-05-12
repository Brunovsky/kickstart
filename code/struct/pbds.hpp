#ifndef PBDS_HPP
#define PBDS_HPP

#include <bits/stdc++.h>

using namespace std;

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

namespace gnu = __gnu_pbds;

template <typename T, typename CompareFn = less<T>>
using ordered_set = gnu::tree<T, gnu::null_type, CompareFn, gnu::rb_tree_tag,
                              gnu::tree_order_statistics_node_update>;

template <typename Key, typename Value, typename CompareFn = less<Key>>
using ordered_map = gnu::tree<Key, Value, CompareFn, gnu::rb_tree_tag,
                              gnu::tree_order_statistics_node_update>;

#include <ext/pb_ds/priority_queue.hpp>

namespace gnu = __gnu_pbds;

template <typename T, typename CompareFn = less<T>>
using min_heap = gnu::priority_queue<T, CompareFn, gnu::pairing_heap_tag>;

#endif // PBDS_HPP
