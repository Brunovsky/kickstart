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

using Int = int;
using Size = size_t;

struct quad_tree;

struct Point {
    Int x, y;
};

struct Box {
    Point min, max;
};

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

int quadrant(Point root, Point point) {
    int dx = root.x <= point.x; // 0 or 1
    int dy = root.y <= point.y; // 0 or 1
    return 2 * dx + dy;
}

bool cmp_x(Point lhs, Point rhs) {
    return lhs.x < rhs.x;
}

bool cmp_y(Point lhs, Point rhs) {
    return lhs.y < rhs.y;
}

void print_box(Box box) {
    printf("[(%+03d,%+03d) (%+03d,%+03d)]", box.min.x, box.min.y, box.max.x, box.max.y);
}

Int manhat_dist(Point lhs, Point rhs) {
    return abs(lhs.x - rhs.x) + abs(lhs.y - rhs.y);
}

Int euclid_dist(Point lhs, Point rhs) {
    Int dx = lhs.x - rhs.x;
    int dy = lhs.y - rhs.y;
    return dx * dx + dy * dy;
}

struct quad_tree {
    using quad_tree_ptr = unique_ptr<quad_tree>;
    quad_tree_ptr children[4];
    Box box;
    Point root;
    Size size = 0;
    Size root_count = 0; // how many instances of the root point we have

    quad_tree(vector<Point> ps, Box input_box) : box(input_box), size(ps.size()) {
        assert(ordered(box.min, box.max));
        assert(size > 0);
        for (Point point : ps) {
            assert(bounded(box, point));
        }

        auto middle = size / 2;
        nth_element(ps.begin(), ps.begin() + middle, ps.end(), cmp_x);
        root.x = ps[middle].x;
        nth_element(ps.begin(), ps.begin() + middle, ps.end(), cmp_y);
        root.y = ps[middle].y;
        box.min = box.max = root;

        vector<Point> buckets[4] = {};
        for (Point point : ps) {
            box.min.x = min(box.min.x, point.x);
            box.min.y = min(box.min.y, point.y);
            box.max.x = max(box.max.x, point.x);
            box.max.y = max(box.max.y, point.y);
            if (point == root) {
                root_count++;
            } else {
                int b = quadrant(root, point) & 3;
                buckets[b].push_back(point);
            }
        }
        for (int b = 0; b < 4; b++) {
            if (!buckets[b].empty()) {
                auto xmin = (b & 2) ? root.x : box.min.x;
                auto xmax = (b & 2) ? box.max.x : root.x;
                auto ymin = (b & 1) ? root.y : box.min.y;
                auto ymax = (b & 1) ? box.max.y : root.y;
                Box child_box = {{xmin, ymin}, {xmax, ymax}};
                children[b] = make_unique<quad_tree>(move(buckets[b]), child_box);
            }
        }
    }

    Size count_query(Box query) const noexcept {
        if (!intersect(query, box)) {
            return 0;
        }
        if (contains(query, box)) {
            return size;
        }
        Size count = bounded(query, root) ? root_count : 0;
        count += children[0] ? children[0]->count_query(query) : 0;
        count += children[1] ? children[1]->count_query(query) : 0;
        count += children[2] ? children[2]->count_query(query) : 0;
        count += children[3] ? children[3]->count_query(query) : 0;
        return count;
    }

    Size depth() const noexcept {
        Size n0 = children[0] ? children[0]->depth() + 1 : 1;
        Size n1 = children[1] ? children[1]->depth() + 1 : 1;
        Size n2 = children[2] ? children[2]->depth() + 1 : 1;
        Size n3 = children[3] ? children[3]->depth() + 1 : 1;
        return max(max(n0, n1), max(n2, n3));
    }
};

} // namespace quad_tree