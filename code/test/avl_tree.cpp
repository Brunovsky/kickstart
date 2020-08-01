#include "../avl_tree.hpp"

#include <algorithm>
#include <random>

// *****

mt19937 mt(random_device{}());

void compare_test() {
    uniform_int_distribution<int> dists(5, 200);
    uniform_int_distribution<int> distn(1, 10000);

    for (int t = 0; t < 2'000; t++) {
        avl_tree<int> lhs, rhs;
        vector<int> nums;
        for (int i = 0, s = dists(mt); i < s; i++) {
            nums.push_back(distn(mt));
        }
        // insert into lhs in one order
        for (int n : nums)
            lhs.insert_unique(n);
        // ... and into rhs in another
        shuffle(begin(nums), end(nums), mt);
        for (int n : nums)
            rhs.insert_unique(n);

        sort(begin(nums), end(nums));
        nums.erase(unique(begin(nums), end(nums)), nums.end());

        assert(lhs.size() == nums.size());
        assert(rhs.size() == nums.size());
        assert(lhs == rhs);
        assert(lhs <= rhs);
        assert(lhs >= rhs);
        assert(!(lhs < rhs));
        assert(!(lhs > rhs));

        lhs.insert_unique(-2); // rhs will surely have a number > 100
        assert(lhs < rhs);

        cout << "\rcompare test #" << t + 1 << flush;
    }
    cout << "\rcompare test OK  -----\n";
}

void battle_test() {
    uniform_int_distribution<int> dists(5, 700);
    uniform_int_distribution<int> distn(-40, 40);
    bernoulli_distribution diste(0.25);

    for (int t = 0; t < 10'000; t++) {
        avl_tree<int> tree;
        assert(tree.empty());

        size_t inserted = 0, not_inserted = 0, erased = 0, not_erased = 0;
        for (int i = 0, s = dists(mt); i < s; i++) {
            int n = distn(mt);
            if (diste(mt)) {
                tree.erase_unique(n) ? erased++ : not_erased++;
                assert(!tree.contains(n));
            } else {
                tree.insert_unique(n).second ? inserted++ : not_inserted++;
                assert(tree.contains(n));
            }
            tree.debug(); // verify invariants
        }
        assert(tree.size() == inserted - erased);
        assert(!tree.empty()); // well, very unlikely...

        vector<int> ltor, rtol;
        for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
            rtol.push_back(*it);
        }
        for (int n : tree) {
            ltor.push_back(n);
        }
        reverse(begin(rtol), end(rtol));
        assert(ltor == rtol && ltor.size() == tree.size());

        cout << "\r battle test #" << t + 1 << flush;
    }
    cout << "\r battle test OK -----\n";
}

int main() {
    compare_test();
    battle_test();
    return 0;
}
