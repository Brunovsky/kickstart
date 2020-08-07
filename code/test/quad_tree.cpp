#include "../quad_tree.hpp"

using namespace chrono;

// *****

auto read_2d_points() {
    ifstream file("datasets/2d_points.txt");
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

void test() {
    Box world = {{-1000000, -100000}, {1000000, 100000}};
    auto points = read_2d_points();

    quad_tree tree(world);
    tree.insert(begin(points), end(points));

    size_t N = points.size();
    double logN = log2(N);

    assert(tree.tree_size() == N);
    assert(tree.count_query(world) == tree.tree_size());

    printf("size: %lu\n", N);
    printf("log2(size): %lf\n", logN);
    printf("depth: %lu\n", tree.depth());

    vector<size_t> correct;
    vector<size_t> answers;
    auto queries = gen_queries(world);

    size_t linear_time = time_vector_query(points, queries, correct);
    size_t tree_time = time_tree_query(tree, queries, answers);

    if (correct != answers) {
        printf("ERROR: quad_tree incorrect query answers\n");
        for (int i = 0; i < QUERIES; i++) {
            if (correct[i] != answers[i]) {
                print_box(queries[i]);
                printf(" %d:\tcorrect:%lu\tanswer:%lu\n", i, correct[i], answers[i]);
            }
        }
        return 1;
    }

    printf("%d queries\n", QUERIES);
    printf("linear time: %lums\n", linear_time);
    printf("tree time:   %lums\n", tree_time);
    printf("size / log2^2(size): %lf\n", N / (logN * logN));
    printf("linear / tree time:  %lf\n", linear_time / double(tree_time));
    printf("quad_tree OK\n");
    return 0;
}

int main() {
    test();
    return 0;
}
