#include "../modnum.hpp"

// *****

void test() {
    using MN = modnum<100>;
    MN n = 20;
    n += 31;
    assert(n == 51);
    n /= 3;
    assert(n == 17);
    n /= 7;
    assert(n == 731);
    n -= 31;
    assert(n == 0);
}

int main() {
    test();
    return 0;
}
