#include "../maximum_matching.hpp"

// *****

template <typename MM>
void test() {
    MM mm(6, 6,
          {
              {1, 1},
              {1, 3},
              {2, 4},
              {2, 5},
              {3, 2},
              {3, 3},
              {4, 3},
              {4, 4},
              {5, 3},
          });
    int maximal = mm.compute();
    printf("maximal #1: %d\n", maximal);
    assert(maximal == 5);

    mm = MM(8, 8,
            {
                {1, 1},
                {1, 4},
                {2, 3},
                {2, 6},
                {2, 7},
                {3, 2},
                {3, 4},
                {3, 5},
                {4, 2},
                {4, 7},
                {5, 5},
                {5, 6},
                {5, 7},
                {6, 3},
                {6, 6},
                {7, 6},
                {7, 7},
            });
    maximal = mm.compute();
    printf("maximal #2: %d\n", maximal);
    assert(maximal == 7);
}

int main() {
    test<maximum_matching>();
    test<hopcroft_karp>();
    return 0;
}
