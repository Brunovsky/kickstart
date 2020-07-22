#include "../sieve.hpp"

int main() {
    N = 40000;
    odd_sieve();
    assert(count_odd_primes(10, 20) == 4);
    assert(count_odd_primes(100, 200) == 21);
    assert(count_odd_primes(1, 9) == 3);

    printf("sieve OK\n");
    return 0;
}
