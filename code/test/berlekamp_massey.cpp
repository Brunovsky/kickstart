#include "test_utils.hpp"
#include "../numeric/berlekamp_massey.hpp"
#include "../numeric/modnum.hpp"

using num = modnum<1'000'000'007>;

void check_bm(vector<num> x) {
    print("{} ==> {}\n", x, berlekamp_massey(x)); //
}

void check_lr(vector<num> x, int64_t k) {
    vector<num> rec = berlekamp_massey(x);
    print("{} ==> {} || {}kth = {}\n", x, rec, k, linear_recurrence(x, rec, k));
}

void unit_test_berlekamp_massey() {
    check_bm({0, 1, 1, 3, 5, 11});                           // 1, 2
    check_bm({0, 1, 4, 7, 21, 50, 127});                     // 1, 3, 2
    check_bm({1, 4, 9, 16, 25, 36, 49, 64, 81, 100});        // 3 -3 1
    check_bm({1, 8, 27, 64, 125, 216, 343, 512, 729, 1000}); // 4 -6 4 -1
    check_bm({0, 1, 5, 14, 30, 55, 91, 140, 204, 285, 385}); // 4 -6 4 -1
    check_bm({0, 1, 9, 36, 100, 225, 441, 784, 1296, 2025, 3025, 4356, 6084, 8281});
    check_lr({0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55}, 40);
}

int main() {
    RUN_SHORT(unit_test_berlekamp_massey());
    return 0;
}
