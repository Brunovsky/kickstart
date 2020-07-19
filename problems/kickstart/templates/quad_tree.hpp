#include <bits/stdc++.h>

using namespace std;

//               y
//       .   .   ↑   .   .
//  .            |            .
//               |
//  .      1     |     3      .
//               |
//    -----------|----------→ x
//               |
//  .      0     |     2      .
//               |
//  .            |            .
//       .   .       .   .

// *****

namespace quad_tree {

constexpr size_t MAX_SIZE_LEAF = 4;

struct Point {
    int x, y;
};

struct Box {
    Point min, max;
};

using PointIt = vector<Point>::iterator;

inline bool operator==(Point lhs, Point rhs) {
    return tie(lhs.x, lhs.y) == tie(rhs.x, rhs.y);
}

inline bool operator<(Point lhs, Point rhs) {
    return tie(lhs.x, lhs.y) < tie(rhs.x, rhs.y);
}

inline bool ordered(Point lo, Point hi) {
    return lo.x <= hi.x && lo.y <= hi.y;
}

inline bool bounded(Box box, Point point) {
    return ordered(box.min, point) && ordered(point, box.max);
}

inline bool intersect(Box aa, Box bb) {
    return ordered(aa.min, bb.max) && ordered(bb.min, aa.max);
}

inline bool contains(Box large, Box small) {
    return ordered(large.min, small.min) && ordered(small.max, large.max);
}

bool cmp_x(Point lhs, Point rhs) {
    return lhs.x < rhs.x;
}

bool cmp_y(Point lhs, Point rhs) {
    return lhs.y < rhs.y;
}

PointIt partition_x(PointIt first, PointIt last, int x) {
    while (first != last && first->x < x)
        ++first;
    if (first == last)
        return first;

    for (auto it = next(first); it != last; ++it) {
        if (it->x < x) {
            iter_swap(it, first);
            ++first;
        }
    }
    return first;
}

PointIt partition_y(PointIt first, PointIt last, int y) {
    while (first != last && first->y < y)
        ++first;
    if (first == last)
        return first;

    for (auto it = next(first); it != last; ++it) {
        if (it->y < y) {
            iter_swap(it, first);
            ++first;
        }
    }
    return first;
}

struct quad_tree {
    unique_ptr<quad_tree> children[4];
    Box box;
    PointIt first, last;

    quad_tree(PointIt first, PointIt last) : first(first), last(last) {
        box = {{INT_MAX, INT_MAX}, {INT_MIN, INT_MIN}};
        for (auto it = first; it != last; ++it) {
            box.min.x = min(box.min.x, it->x);
            box.min.y = min(box.min.y, it->y);
            box.max.x = max(box.max.x, it->x);
            box.max.y = max(box.max.y, it->y);
        }
        size_t size = last - first;

        if (size <= MAX_SIZE_LEAF) {
            return;
        }

        auto middle = size / 2;
        Point root;
        nth_element(first, first + middle, last, cmp_x);
        root.x = (first + middle)->x;
        nth_element(first, first + middle, last, cmp_y);
        root.y = (first + middle)->y;

        auto nth2 = partition_x(first, last, root.x);
        auto nth1 = partition_y(first, nth2, root.y);
        auto nth3 = partition_y(nth2, last, root.y);

        children[0] = make_unique<quad_tree>(first, nth1);
        children[1] = make_unique<quad_tree>(nth1, nth2);
        children[2] = make_unique<quad_tree>(nth2, nth3);
        children[3] = make_unique<quad_tree>(nth3, last);
    }

    size_t count_query(Box query) const noexcept {
        if (first == last || !intersect(query, box)) {
            return 0;
        }
        if (contains(query, box)) {
            return last - first;
        }
        size_t count = 0;
        if (!children[0]) {
            for (auto it = first; it != last; ++it) {
                count += bounded(query, *it);
            }
        } else {
            count += children[0]->count_query(query);
            count += children[1]->count_query(query);
            count += children[2]->count_query(query);
            count += children[3]->count_query(query);
        }
        return count;
    }

    size_t depth() const noexcept {
        if (!children[0]) {
            return 1;
        } else {
            auto n0 = children[0]->depth() + 1;
            auto n1 = children[1]->depth() + 1;
            auto n2 = children[2]->depth() + 1;
            auto n3 = children[3]->depth() + 1;
            return max(max(n0, n1), max(n2, n3));
        }
    }

    size_t size() const noexcept {
        return last - first;
    }
};

struct binary_quad_tree {
    unique_ptr<binary_quad_tree> children[4];
    Box box;
    PointIt first, last;

    binary_quad_tree(PointIt first, PointIt last) : first(first), last(last) {
        box = {{INT_MAX, INT_MAX}, {INT_MIN, INT_MIN}};
        for (auto it = first; it != last; ++it) {
            box.min.x = min(box.min.x, it->x);
            box.min.y = min(box.min.y, it->y);
            box.max.x = max(box.max.x, it->x);
            box.max.y = max(box.max.y, it->y);
        }
        size_t size = last - first;
        Point root = {(box.min.x + box.max.x) / 2, (box.min.y + box.max.y) / 2};

        if (size <= MAX_SIZE_LEAF || root == box.min) {
            return;
        }

        auto nth2 = partition_x(first, last, root.x);
        auto nth1 = partition_y(first, nth2, root.y);
        auto nth3 = partition_y(nth2, last, root.y);

        children[0] = make_unique<binary_quad_tree>(first, nth1);
        children[1] = make_unique<binary_quad_tree>(nth1, nth2);
        children[2] = make_unique<binary_quad_tree>(nth2, nth3);
        children[3] = make_unique<binary_quad_tree>(nth3, last);
    }

    size_t count_query(Box query) const noexcept {
        if (first == last || !intersect(query, box)) {
            return 0;
        }
        if (contains(query, box)) {
            return last - first;
        }
        size_t count = 0;
        if (!children[0]) {
            for (auto it = first; it != last; ++it) {
                count += bounded(query, *it);
            }
        } else {
            count += children[0]->count_query(query);
            count += children[1]->count_query(query);
            count += children[2]->count_query(query);
            count += children[3]->count_query(query);
        }
        return count;
    }

    size_t depth() const noexcept {
        if (!children[0]) {
            return 1;
        } else {
            auto n0 = children[0]->depth() + 1;
            auto n1 = children[1]->depth() + 1;
            auto n2 = children[2]->depth() + 1;
            auto n3 = children[3]->depth() + 1;
            return max(max(n0, n1), max(n2, n3));
        }
    }

    size_t size() const noexcept {
        return last - first;
    }
};

} // namespace quad_tree
