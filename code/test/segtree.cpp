#include "test_utils.hpp"
#include "../struct/segtree.hpp"
#include "../struct/dyn_segtree.hpp"
#include "../struct/sparse_segtree.hpp"

inline namespace unit_testing_segtree {

void unit_test_add_segtree() {
    using namespace samples_segtree;
    using sum_segnode = sum_segnode;
    using add_segupdate = add_segupdate;

    vector<int> arr = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    int N = arr.size();

    segtree<sum_segnode, add_segupdate> tree(0, N, arr);
    assert(tree.query_range(0, 7).value == 42);

    tree.update_range(0, 3, 2);
    assert(tree.query_range(0, 7).value == 48);
    assert(tree.query_range(2, 3).value == 9);

    tree.update_range(4, 5, 10);
    assert(tree.query_range(0, 7).value == 58);
}

void unit_test_dyn_segtree() {
    using namespace samples_dyn_segtree;
    using sum_segnode = sum_segnode;
    using add_segupdate = add_segupdate;

    dyn_segtree<sum_segnode> tree(10, 30);

    tree.update(14, add_segupdate(1));
    tree.update(13, add_segupdate(2));
    tree.update(27, add_segupdate(3));
    tree.update(21, add_segupdate(4));
    tree.update(24, add_segupdate(5));
    tree.update(18, add_segupdate(6));
    tree.update(21, add_segupdate(7));
    assert(tree.query(21, 22).value == 11);
    assert(tree.query(14, 18).value == 1);
    assert(tree.query(13, 19).value == 9);
    assert(tree.query(10, 30).value == 28);
}

void unit_test_sparse_segtree() {
    using namespace samples_sparse_segtree;
    using sum_segnode = sum_segnode;
    using add_segupdate = add_segupdate;

    sparse_segtree<sum_segnode, add_segupdate> tree(1, 300'000'000);

    tree.update_range(1'000'000, 5'000'000, 2);
    assert(tree.query_range(2'000'000, 3'000'000).value == 2'000'000);
    // assert(tree.query_range(500'000, 4'000'000).value == 6'000'000);
    // assert(tree.query_range(4'000'000, 6'000'000).value == 2'000'000);
    tree.update_range(15'000'000, 20'000'000, 3);
    // assert(tree.query_range(10'000'000, 12'000'000).value == 0);
    // assert(tree.query_range(4'000'000, 16'000'000).value == 5'000'000);
}

} // namespace unit_testing_segtree

inline namespace stress_testing_segtree {

template <typename SegTree>
void stress_test_segtree_run() {
    constexpr int N = 200;
    intd vald(-20, 20);
    int arr[N] = {};
    SegTree tree(0, N);

    enum ActionsSegtree {
        CHECK_ONE,
        CHECK_ALL,
        ADD,
        QUERY,
    };

    discrete_distribution<int> actiond({50, 10, 5000, 5000});

    LOOP_FOR_DURATION_TRACKED_RUNS(5s, now, runs) {
        print_time(now, 5s, 50ms, "stress test segtree ({} runs)", runs);

        auto action = ActionsSegtree(actiond(mt));

        switch (action) {
        case CHECK_ONE: {
            for (int u = 0; u < N; u++) {
                assert(tree.query_range(u, u + 1).value == arr[u]);
            }
        } break;
        case CHECK_ALL: {
            for (int u = 0; u < N; u++) {
                int sum = 0;
                for (int v = u + 1; v < N; v++) {
                    sum += arr[v - 1];
                    assert(tree.query_range(u, v).value == sum);
                }
            }
        } break;
        case ADD: {
            auto [L, R] = different(0, N);
            assert(L < R);
            int val = vald(mt);
            tree.update_range(L, R, val);
            for (int u = L; u < R; u++) {
                arr[u] += val;
            }
        } break;
        case QUERY: {
            auto [L, R] = different(0, N);
            assert(L < R);
            auto v0 = tree.query_range(L, R).value;
            auto v1 = accumulate(arr + L, arr + R, 0);
            assert(v0 == v1);
        } break;
        }
    }

    printcl("total runs: {}\n", runs);
}

void stress_test_segtree() {
    namespace st = samples_segtree;
    namespace ss = samples_sparse_segtree;

    stress_test_segtree_run<segtree<st::sum_segnode, st::add_segupdate>>();
    stress_test_segtree_run<sparse_segtree<ss::sum_segnode, ss::add_segupdate>>();
}

} // namespace stress_testing_segtree

int main() {
    RUN_SHORT(unit_test_add_segtree());
    RUN_SHORT(unit_test_dyn_segtree());
    RUN_SHORT(unit_test_sparse_segtree());
    RUN_SHORT(stress_test_segtree());
    return 0;
}
