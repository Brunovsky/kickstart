#include "disjoint_set.hpp"
#include "hash.hpp"
#include "kmp.hpp"
#include "math.hpp"
#include "sieve.hpp"
#include "fenwick.hpp"
#include "segment_tree.hpp"
#include "quad_tree.hpp"

namespace quad_tree {

using namespace chrono;

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
    size_t count = 0;
    for (Point point : points) {
        count += bounded(query, point);
    }
    return count;
}

#define QUERIES 500

void print_box(Box box) {
    printf("[(%+6d,%+6d) (%+6d,%+6d)]", box.min.x, box.min.y, box.max.x, box.max.y);
}

auto gen_queries(Box box) {
    printf("world: "), print_box(box), printf("\n");

    mt19937 mt(random_device{}());
    uniform_int_distribution<int> Xdist(box.min.x, box.max.x);
    uniform_int_distribution<int> Ydist(box.min.y, box.max.y);

    vector<Box> queries(QUERIES);
    for (int i = 0; i < QUERIES; i++) {
        int x1 = Xdist(mt);
        int x2 = Xdist(mt);
        int y1 = Ydist(mt);
        int y2 = Ydist(mt);
        queries[i].min.x = min(x1, x2);
        queries[i].min.y = min(y1, y2);
        queries[i].max.x = max(x1, x2);
        queries[i].max.y = max(y1, y2);
    }
    return queries;
}

size_t time_vector_query(const vector<Point> &points, const vector<Box> &queries,
                         vector<size_t> &correct) {
    correct.resize(QUERIES);
    auto begin = steady_clock::now();
    for (int i = 0; i < QUERIES; i++) {
        correct[i] = query_vector(points, queries[i]);
    }
    return duration_cast<milliseconds>(steady_clock::now() - begin).count();
}

template <typename Tree>
size_t time_tree_query(const Tree &tree, const vector<Box> &queries,
                       vector<size_t> &answers) {
    answers.resize(QUERIES);
    auto begin = steady_clock::now();
    for (int i = 0; i < QUERIES; i++) {
        answers[i] = tree.count_query(queries[i]);
    }
    return duration_cast<milliseconds>(steady_clock::now() - begin).count();
}

void test_quad_tree() {
    auto points = read_2d_points();
    quad_tree tree(points.begin(), points.end());
    size_t N = points.size();
    double logN = log2(N);

    assert(tree.size() == N);
    assert(tree.count_query(tree.box) == tree.size());

    printf("size: %lu\n", N);
    printf("log2(size): %lf\n", logN);
    printf("depth: %lu\n", tree.depth());

    vector<size_t> correct;
    vector<size_t> answers;
    auto queries = gen_queries(tree.box);

    size_t linear_time = time_vector_query(points, queries, correct);
    size_t tree_time = time_tree_query(tree, queries, answers);

    if (correct != answers) {
        printf("ERROR: quad_tree incorrect answers\n");
        for (int i = 0; i < QUERIES; i++) {
            if (correct[i] != answers[i]) {
                print_box(queries[i]);
                printf(" %d:\tcorrect:%lu\tanswer:%lu\n", i, correct[i], answers[i]);
            }
        }
        return;
    }

    printf("%d queries\n", QUERIES);
    printf("linear time: %lums\n", linear_time);
    printf("tree time:   %lums\n", tree_time);
    printf("size / log2^2(size): %lf\n", N / (logN * logN));
    printf("linear / tree time:  %lf\n", linear_time / double(tree_time));
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
