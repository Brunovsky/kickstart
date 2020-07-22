#include <bits/stdc++.h>

using namespace std;

// *****

constexpr size_t MAX_SZ_LEAF = 4;

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

// 3 Query operations
using Query = Box;

inline bool intersect(Query query, Box box) {
    return ordered(query.min, box.max) && ordered(box.min, query.max);
}

inline bool contains(Query query, Box box) {
    return ordered(query.min, box.min) && ordered(box.max, query.max);
}

inline bool bounded(Query query, Point point) {
    return ordered(query.min, point) && ordered(point, query.max);
}

struct quad_tree {
    array<unique_ptr<quad_tree>, 4> children;
    Box box;
    size_t size = 0;
    vector<Point> points;

    bool is_partitioned() const noexcept {
        return bool(children[0]);
    }

    bool is_singular() const noexcept {
        return box.min == box.max;
    }

    Point root_point() const noexcept {
        return {(box.min.x + box.max.x + 1) >> 1, (box.min.y + box.max.y + 1) >> 1};
    }

    void partition_tree() noexcept {
        assert(!is_partitioned() && !is_singular());
        auto root = root_point();
        Box box0 = {{box.min.x, box.min.y}, {root.x - 1, root.y - 1}};
        Box box1 = {{box.min.x, root.y}, {root.x - 1, box.max.y}};
        Box box2 = {{root.x, box.min.y}, {box.max.x, root.y - 1}};
        Box box3 = {{root.x, root.y}, {box.max.x, box.max.y}};
        children[0] = make_unique<quad_tree>(box0);
        children[1] = make_unique<quad_tree>(box1);
        children[2] = make_unique<quad_tree>(box2);
        children[3] = make_unique<quad_tree>(box3);

        partition_insert(begin(points), end(points));
        points.clear();
    }

    void partition_insert(PointIt first, PointIt last) {
        assert(is_partitioned());
        auto root = root_point();
        auto nth2 = partition_x(first, last, root.x);
        auto nth1 = partition_y(first, nth2, root.y);
        auto nth3 = partition_y(nth2, last, root.y);
        children[0]->insert(first, nth1);
        children[1]->insert(nth1, nth2);
        children[2]->insert(nth2, nth3);
        children[3]->insert(nth3, last);
    }

    quad_tree(Box box) : box(box), size(0) {}

    void clear() noexcept {
        children[0] = nullptr;
        children[1] = nullptr;
        children[2] = nullptr;
        children[3] = nullptr;
        size = 0;
        points.clear();
    }

    void insert(PointIt first, PointIt last) {
        if (first == last) {
            return;
        }
        size += last - first;

        if (is_singular()) {
            return;
        }

        if (is_partitioned()) {
            partition_insert(first, last);
            return;
        }

        copy(first, last, back_inserter(points));
        partition_tree();
        partition_insert(begin(points), end(points));
    }

    size_t count_query(Query query) const noexcept {
        if (size == 0 || !intersect(query, box)) {
            return 0;
        }
        if (contains(query, box)) {
            return size;
        }
        size_t count = 0;
        if (!is_partitioned()) {
            for (auto point : points) {
                count += bounded(query, point);
            }
        } else {
            count += children[0]->count_query(query);
            count += children[1]->count_query(query);
            count += children[2]->count_query(query);
            count += children[3]->count_query(query);
        }
        return count;
    }

    size_t tree_size() const noexcept {
        return size;
    }

    size_t depth() const noexcept {
        if (is_partitioned()) {
            return 1 + max(max(children[0]->depth(), children[1]->depth()),
                           max(children[2]->depth(), children[3]->depth()));
        }
        return 1;
    }
};

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
