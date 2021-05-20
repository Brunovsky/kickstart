#include "test_utils.hpp"
#include "../struct/integer_trees.hpp"

inline namespace unit_testing_interval_tree {

void unit_test_merging_interval_tree() {
    merging_interval_tree<double> tree;

    tree.insert({10, 40});
    print("tree: {}\n", tree);
    tree.insert({100, 115});
    print("tree: {}\n", tree);
    tree.insert({130, 145});
    print("tree: {}\n", tree);
    tree.insert({60, 80});
    print("tree: {}\n", tree);
    tree.insert({155, 180});
    print("tree: {}\n", tree);
    tree.insert({82, 90});
    print("tree: {}\n", tree);
    tree.insert({65, 112});
    print("tree: {}\n", tree);
    tree.exclude({70, 110});
    print("tree: {}\n", tree);

    print("cover: {}\n", tree.cover_length());
}

} // namespace unit_testing_interval_tree

inline namespace stress_testing_interval_tree {

enum MergingAction {
    INSERT,
    EXCLUDE,
    CONTAINS,
    COVER_LENGTH,
    SIZE,
};

void stress_test_merging_interval_tree(int T = 300'000) {
    discrete_distribution<int> actiond({100, 100, 1000, 50, 0});
    int N = 1'000'000, k = 2'000;
    intd numd(0, N);
    intd rang(0, N - k);
    intd lend(1, k);

    set<int> nums;
    merging_interval_tree<int> tree;

    for (int t = 0; t < T; t++) {
        if (t % 100 == 0) {
            print_progress(t, T, "stress test merging interval tree {:7} {:3}",
                           nums.size(), tree.size());
        }

        auto action = MergingAction(actiond(mt));

        switch (action) {
        case INSERT: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            tree.insert({lo, hi});
            auto it = nums.lower_bound(hi);
            for (int n = hi - 1; n >= lo; n--) {
                it = nums.insert(it, n);
            }
        } break;
        case EXCLUDE: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            tree.exclude({lo, hi});
            auto it0 = nums.lower_bound(lo);
            auto it1 = nums.lower_bound(hi);
            nums.erase(it0, it1);
        } break;
        case CONTAINS: {
            int n = numd(mt);
            assert(tree.contains(n) == nums.count(n));
        } break;
        case COVER_LENGTH: {
            assert(tree.cover_length() == int(nums.size()));
        } break;
        default:
            print("Unsupported action: {}\n", int(action));
            abort();
        }

        if (!tree.verify()) {
            print("tree: {}\n", tree);
        }
        assert(tree.verify());
    }
}

} // namespace stress_testing_interval_tree

int main() {
    RUN_SHORT(unit_test_merging_interval_tree());
    RUN_BLOCK(stress_test_merging_interval_tree());
    return 0;
}
