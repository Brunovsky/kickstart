#include "disjoint_set.hpp"
#include "hash.hpp"
#include "kmp.hpp"
#include "math.hpp"
#include "sieve.hpp"
#include "fenwick.hpp"
#include "segment_tree.hpp"
#include "quad_tree.hpp"

namespace quad_tree {

auto read_2d_points() {
    ifstream file("2d_points.txt");
    assert(file.is_open());
    int N;
    file >> N >> ws;
    vector<Point> points(N);
    for (int i = 0; i < N; i++) {
        file >> points[i].x >> points[i].y >> ws;
    }
    return points;
}

auto query_vector(const vector<Point> &points, Box query) {
    Size count = 0;
    for (Point point : points) {
        count += bounded(query, point);
    }
    return count;
}

auto gen_queries(Box box) {
    const int QUERIES = 1000;
    printf("World: "), print_box(box), printf("\n");
    random_device rng;
    mt19937 mt(rng());
    uniform_int_distribution<Int> Xdist(box.min.x, box.max.x);
    uniform_int_distribution<Int> Ydist(box.min.y, box.max.y);
    vector<Box> queries(QUERIES);
    for (int i = 0; i < QUERIES; i++) {
        Box query;
        int x1, x2, y1, y2;
        x1 = Xdist(mt);
        x2 = Xdist(mt);
        y1 = Ydist(mt);
        y2 = Ydist(mt);
        query.min.x = min(x1, x2);
        query.min.y = min(y1, y2);
        query.max.x = max(x1, x2);
        query.max.y = max(y1, y2);
        queries[i] = query;
    }
    return queries;
}

void test_quad_tree() {
    auto points = read_2d_points();
    quad_tree tree(points, {{INT_MIN, INT_MIN}, {INT_MAX, INT_MAX}});

    assert(tree.size == points.size());
    assert(tree.count_query(tree.box) == tree.size);

    printf("size: %lu\n", tree.size);
    printf("log2(size): %lf\n", log2(tree.size) / 2.0);
    printf("depth: %lu\n", tree.depth());

    auto queries = gen_queries(tree.box);
    for (Box box : queries) {
        Size c = tree.count_query(box);
        assert(query_vector(points, box) == c);
    }
    printf("quad_tree OK\n");
}

} // namespace quad_tree

int main() {
    assert(math::power(3, 17, 5) == 3);
    assert(math::power(3, 17) == 129140163);
    assert(math::gcd(135, 54) == 27);
    assert(math::gcd(135, -54) == 27);
    assert(math::gcd(-135, -54) == 27);
    assert(math::totient(35) == 24);
    assert(math::totient(70) == 24);
    assert(math::totient(97) == 96);
    assert(math::totient(194) == 96);
    assert(math::totient(73) == 72);
    assert(math::totient(48) == 16);
    assert(math::binomial(4, 2) == 6);
    assert(math::binomial(6, 3) == 20);
    assert(math::binomial(6, 2) == 15);
    assert(math::binomial(2, 0) == 1);
    assert(math::binomial(5, 2) == 10);
    assert(math::binomial(2, 2) == 1);
    assert(math::binomial(3, 1) == 3);

    sieve::N = 40000;
    sieve::odd_sieve();
    assert(sieve::count_odd_primes(10, 20) == 4);
    assert(sieve::count_odd_primes(100, 200) == 21);
    assert(sieve::count_odd_primes(1, 9) == 3);

    math::i64 x, y;
    assert(math::gcd(54, 135, x, y) == 27);
    assert(54 * x + 135 * y == 27);
    assert(math::gcd(54, -135, x, y) == 27);
    assert(54 * x + -135 * y == 27);
    assert(math::gcd(-54, 135, x, y) == 27);
    assert(-54 * x + 135 * y == 27);
    assert(math::gcd(-54, -135, x, y) == 27);
    assert(-54 * x + -135 * y == 27);
    assert(math::gcd(13121, 17431, x, y) == 1);
    assert(math::invmod(3, 17) == 6);
    assert(math::invmod(4, 17) == 13);

    quad_tree::test_quad_tree();

    return 0;
}
