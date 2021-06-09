#include "test_utils.hpp"
#include "../struct/integer_trees.hpp"
#include "../struct/dynamic_cht.hpp"

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

void unit_test_dynamic_cht() {
    dynamic_cht<long> cht;
    cht.add(2, -3);
    cht.add(4, -70);
    cht.add(-20, -500);
    cht.add(-5, -100);
    cht.add(-8, -200);
    cht.add(10, -100);
    cht.add(-1, 100);
    cout << cht.size() << endl;
    for (auto [m, b, end] : cht) {
        print("({}, {}, {})\n", m, b, end);
    }
}

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

    print("cover: {}\n", tree.cover_length());
}

} // namespace unit_testing_interval_tree

inline namespace stress_testing_interval_tree {

enum MergingAction {
    INSERT,
    EXCLUDE,
    CONTAINS,
    CONTAINS_INTERVAL,
    OVERLAPS_INTERVAL,
    COVER_LENGTH,
    END,
};

constexpr int arr[END] = {200, 200, 500, 200, 200, 150};

bool set_overlaps(const set<int>& nums, int lo, int hi) {
    auto it = nums.lower_bound(lo);
    return it != nums.end() && *it < hi;
}

bool set_contains(const set<int>& nums, int lo, int hi) {
    auto it0 = nums.lower_bound(lo);
    auto it1 = nums.lower_bound(hi - 1);
    return it0 != nums.end() && *it0 == lo &&     //
           it1 != nums.end() && *it1 == hi - 1 && //
           distance(it0, it1) == hi - lo - 1;
}

void stress_test_merging_interval_tree(int N = 100'000, int k = 200) {
    intd numd(0, N);
    intd rang(0, N - k);
    intd lend(1, k);

    discrete_distribution<int> actiond(arr, arr + END);
    set<int> nums;
    merging_interval_tree<int> mit;

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, 50ms, "stress test merging interval tree {:7} {:3}",
                   nums.size(), mit.size());

        auto action = MergingAction(actiond(mt));

        switch (action) {
        case INSERT: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            mit.add({lo, hi});
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
        case CONTAINS: {
            int n = numd(mt);
            assert(mit.contains(n) == nums.count(n));
        } break;
        case CONTAINS_INTERVAL: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            assert(mit.contains({lo, hi}) == set_contains(nums, lo, hi));
        } break;
        case OVERLAPS_INTERVAL: {
            int lo = rang(mt);
            int hi = lo + lend(mt);
            assert(mit.overlaps({lo, hi}) == set_overlaps(nums, lo, hi));
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
    RUN_SHORT(unit_test_dynamic_cht());
    RUN_SHORT(unit_test_merging_interval_tree());
    RUN_BLOCK(stress_test_merging_interval_tree(3000, 50));
    RUN_BLOCK(stress_test_merging_interval_tree(100'000, 500));
    return 0;
}
