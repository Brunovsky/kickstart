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

} // namespace detail

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

void stress_test_merging_interval_tree(int N, int k) {
    // ints [0,N], max interval size of k for one op.
    intd numd(0, N);
    intd rang(0, N - k);
    intd lend(1, k);

    discrete_distribution<int> actiond(arr, arr + END);
    set<int> nums;
    merging_interval_tree<int> mit;

    LOOP_FOR_DURATION_TRACKED_RUNS (4s, now, runs) {
        print_time(now, 4s, "stress test MIT {:7} {:3}", nums.size(), mit.size());

        auto action = actiond(mt);

        switch (MergingAction(action)) {
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
            print("Unsupported action: {}\n", action);
            abort();
        }

        assert(verify(mit));
    }
}

void unit_test_rollback_setmap() {
    map_rollback<int, string> rmap;
    multiset_rollback<string> rset;

    vector<map<int, string>> map_saves;
    vector<multiset<string>> set_saves;
    vector<int> map_times;
    vector<int> set_times;

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 10; j++) {
            int k = rand_unif<int>(0, 1000);
            if (boold(0.55)(mt)) {
                rmap[k] = rand_string(4, 'a', 'm');
            } else {
                rmap.erase(k);
            }
            string s = rand_string(5, 'a', 'd');
            if (boold(0.55)(mt)) {
                rset.insert(s);
            } else {
                rset.erase(s);
            }
        }
        map_saves.push_back(rmap);
        set_saves.push_back(rset);
        map_times.push_back(rmap.time());
        set_times.push_back(rset.time());
    }

    for (int i = 99; i >= 0; i--) {
        rmap.rollback(map_times[i]);
        assert(rmap == map_saves[i]);
        rset.rollback(set_times[i]);
        assert(rset == set_saves[i]);
    }
}

int main() {
    RUN_SHORT(unit_test_merging_interval_tree());
    RUN_SHORT(unit_test_rollback_setmap());
    RUN_BLOCK(stress_test_merging_interval_tree(3000, 50));
    RUN_BLOCK(stress_test_merging_interval_tree(100'000, 500));
    return 0;
}
