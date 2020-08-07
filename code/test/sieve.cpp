#include "../sieve.hpp"

// *****

void test() {
    N = 40000;
    odd_sieve();
    assert(count_odd_primes(10, 20) == 4);
    assert(count_odd_primes(100, 200) == 21);
    assert(count_odd_primes(1, 9) == 3);
}

int main() {
    test();
    return 0;
}
