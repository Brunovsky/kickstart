#include "test_utils.hpp"
#include "../struct/rmq.hpp"

void unit_test_rmq() {
    min_rmq<int> rmq({3, 9, 2, 7, 8, 4, 7, 3, 1, 6, 8, 2});
    assert(rmq.query(0, 12) == 1);
    assert(rmq.query(2, 8) == 2);
    assert(rmq.query(2, 10) == 1);
    assert(rmq.query(5, 7) == 4);
}

void unit_test_rmq_index() {
    min_rmq_index<int> rmq({3, 9, 2, 7, 8, 4, 7, 3, 1, 6, 8, 2});
    assert(rmq.query(0, 12) == 8);
    assert(rmq.query(2, 8) == 2);
    assert(rmq.query(2, 10) == 8);
    assert(rmq.query(5, 7) == 5);
}

int main() {
    RUN_SHORT(unit_test_rmq());
    RUN_SHORT(unit_test_rmq_index());
    return 0;
}
