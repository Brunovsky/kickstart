#ifdef NDEBUG
#undef NDEBUG
#endif

#include <algorithm>
#include <iostream>
#include <random>

#include "../bs_set.hpp"

using namespace std;

// *****

/**
 * Simplified version of bs_tree.cpp test suite with just the basic insert and erase
 * operations, to test new cores (avl and rb)
 */

template struct bs_set<int>;

mt19937 mt(random_device{}());
using intd = uniform_int_distribution<int>;
using boold = bernoulli_distribution;

void insert_test(int T = 500) {
    intd distn(1, 999);
    intd dists(0, 500);

    for (int t = 1; t <= T; t++) {
        bs_set<int> tree;
        for (int i = 0, s = dists(mt); i < s; i++) {
            tree.insert(distn(mt));
            tree.debug();
        }
        cout << "\r      insert test #" << t << flush;
    }
    cout << "\r      insert test OK -----\n";
}

void erase_test(int T = 500) {
    intd distn(1, 999);
    intd dists(0, 500);

    for (int t = 1; t <= T; t++) {
        bs_set<int> tree;
        vector<int> nums;
        int s = dists(mt);
        for (int i = 0; i < s; i++) {
            int n = distn(mt);
            nums.push_back(n);
            tree.insert(n);
        }
        tree.debug();
        shuffle(begin(nums), end(nums), mt);
        for (int i = 0; i < s; i++) {
            tree.erase(nums[i]);
            tree.debug();
        }
        assert(tree.empty());
        cout << "\r       erase test #" << t << flush;
    }
    cout << "\r       erase test OK -----\n";
}

int main() {
    insert_test();
    erase_test();
    return 0;
}
