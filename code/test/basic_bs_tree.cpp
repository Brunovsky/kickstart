#undef NDEBUG

#include "../bstree/bs_set.hpp"
#include "../bstree/bs_tree_debug.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

using namespace std;

// *****

/**
 * Simplified version of bs_tree.cpp test suite with just the basic insert and erase
 * operations, to test new or buggy cores (splay)
 */

template struct bs_set<int>;

void insert_test(int T = 1000) {
    intd distn(1, 999);
    intd dists(0, 3000);

    for (int t = 0; t < T; t++) {
        bs_set<int> tree;
        for (int i = 0, s = dists(mt); i < s; i++) {
            tree.insert(distn(mt));
        }
        debug_tree(tree).debug();
        print_progress(t, T, "insert");
    }
}

void erase_test(int T = 300) {
    intd distn(1, 999);
    intd dists(0, 3000);

    for (int t = 0; t < T; t++) {
        bs_set<int> tree;
        vector<int> nums;
        int s = dists(mt);
        for (int i = 0; i < s; i++) {
            int n = distn(mt);
            nums.push_back(n);
            tree.insert(n);
        }
        debug_tree(tree).debug();
        shuffle(begin(nums), end(nums), mt);
        for (int i = 0; i < s; i++) {
            tree.erase(nums[i]);
            debug_tree(tree).debug();
        }
        assert(tree.empty());
        print_progress(t, T, "erase");
    }
}

int main() {
    RUN_SHORT(insert_test());
    RUN_SHORT(erase_test());
    return 0;
}
