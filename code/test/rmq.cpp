#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../rmq.hpp"

// *****

void unit_test_rmq() {
    RMQ<int> rmq({3, 9, 2, 7, 8, 4, 7, 3, 1, 6, 8, 2});
    assert(rmq.query(0, 12) == 1);
    assert(rmq.query(2, 8) == 2);
    assert(rmq.query(2, 10) == 1);
    assert(rmq.query(5, 7) == 4);
}

int main() {
    unit_test_rmq();
    return 0;
}
