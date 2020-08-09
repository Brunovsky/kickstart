#include "../travelling_salesman.hpp"

// *****

void print_path(const vector<int>& trip) {
    printf("trip:");
    for (int v : trip)
        printf(" %d", v);
    printf("\n");
}

void test() {
    travelling_salesman tsp;
    tsp.V = 4;
    tsp.dist = {
        {0, 2, 9, 10},
        {1, 0, 6, 4},
        {15, 7, 0, 8},
        {6, 3, 12, 0},
    };
    auto res = tsp.compute();
    printf("optimum #1: %ld\n", res.first);
    print_path(res.second);
    tsp.dist = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0},
    };
    res = tsp.compute();
    printf("optimum #2: %ld\n", res.first);
    print_path(res.second);
}

int main() {
    test();
    return 0;
}
