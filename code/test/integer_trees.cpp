#include "test_utils.hpp"
#include "../struct/integer_trees.hpp"

inline namespace detail {

template <typename T>
bool verify(const merging_interval_tree<T>& mit) {
    if (!mit.empty()) {
        auto [lo, hi] = *mit.begin();
        if (lo >= hi) {
            return false;
        }
        for (auto it = next(mit.begin()); it != mit.end(); ++it) {
            auto [nlo, nhi] = *it;
            if (hi >= nlo || nlo >= nhi) {
                return false;
            }
            tie(lo, hi) = make_pair(nlo, nhi);
        }
    }
    return true;
}

} // namespace detail

inline namespace unit_testing_interval_tree {

void unit_test_merging_interval_tree() {
    merging_interval_tree<int> tree;

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
    tree.toggle({60, 70});
    print("tree: {}\n", tree);
    tree.toggle({135, 140});
    print("tree: {}\n", tree);
    tree.toggle({70, 155});
    print("tree: {}\n", tree);

    tree.toggle({25, 50});
    print("tree: {}\n", tree);
    tree.toggle({50, 65});
    print("tree: {}\n", tree);
    tree.toggle({65, 70});
    print("tree: {}\n", tree);
    tree.toggle({115, 140});
    print("tree: {}\n", tree);

    print("cover: {}\n", tree.cover_length());
}

} // namespace unit_testing_interval_tree

inline namespace stress_testing_interval_tree {

enum MergingAction {
    INSERT,
    EXCLUDE,
    TOGGLE,
    CONTAINS,
    COVER_LENGTH,
    END,
};

constexpr int arr[END] = {100, 100, 400, 1000, 200};

void stress_test_merging_interval_tree(int N = 100'000, int k = 200) {
    intd numd(0, N);
    intd rang(0, N - k);
    intd lend(1, k);

    discrete_distribution<int> actiond(arr, arr + END);
    set<int> nums;
    merging_interval_tree<int> mit;

    LOOP_FOR_DURATION_OR_RUNS_TRACKED(10s, now, 400'000, runs) {
        print_time(now, 10s, 50ms, "stress test merging interval tree {:7} {:3}",
                   nums.size(), mit.size());

        auto action = MergingAction(actiond(mt));

        switch (action) {
        case INSERT: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            mit.insert({lo, hi});
            auto it = nums.lower_bound(hi);
            for (int n = hi - 1; n >= lo; n--) {
                it = nums.insert(it, n);
            }
        } break;
        case EXCLUDE: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            mit.exclude({lo, hi});
            auto it0 = nums.lower_bound(lo);
            auto it1 = nums.lower_bound(hi);
            nums.erase(it0, it1);
        } break;
        case TOGGLE: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            mit.toggle({lo, hi});
            auto it0 = nums.lower_bound(lo);
            auto it1 = nums.lower_bound(hi);
            vector<int> exists(it0, it1);
            nums.erase(it0, it1);
            auto it = nums.end();
            for (int E = exists.size(), i = E - 1, n = hi - 1; n >= lo; n--) {
                if (i >= 0 && exists[i] == n) {
                    i--;
                } else {
                    it = nums.insert(it, n);
                }
            }
        } break;
        case CONTAINS: {
            int n = numd(mt);
            assert(mit.contains(n) == nums.count(n));
        } break;
        case COVER_LENGTH: {
            assert(mit.cover_length() == int(nums.size()));
        } break;
        default:
            print("Unsupported action: {}\n", int(action));
            abort();
        }

        assert(verify(mit));
    }
}

} // namespace stress_testing_interval_tree

int main() {
    RUN_SHORT(unit_test_merging_interval_tree());
    RUN_BLOCK(stress_test_merging_interval_tree(3000, 50));
    RUN_BLOCK(stress_test_merging_interval_tree(100'000, 500));
    return 0;
}
