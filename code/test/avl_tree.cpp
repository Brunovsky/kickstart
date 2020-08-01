#include "../avl_tree.hpp"

#include <random>

// *****

mt19937 mt(random_device{}());
uniform_int_distribution<int> dist(1, 40);
bernoulli_distribution doErase(0.25);

void test() {
    for (int k = 0; k < 100; k++) {
        avl_tree<int> tree;
        assert(tree.empty());
        printf("test %d\n", k + 1);
        for (int i = 0; i < 2000; i++) {
            int n = dist(mt);
            if (doErase(mt)) {
                tree.erase(n);
            } else {
                tree.insert(n);
            }
            tree.debug();
        }
        assert(!tree.empty());
    }
    printf("\n");
}

int main() {
    test();
    return 0;
}
