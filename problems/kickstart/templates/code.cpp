#include "hash.hpp"
#include "math.hpp"
#include "sieve.hpp"
#include "static_fenwick.hpp"
#include "static_segment_tree.hpp"

int main() {
    assert(math::power(3, 17, 5) == 3);
    assert(math::power(3, 17) == 129140163);
    assert(math::gcd(135, 54) == 27);
    assert(math::phi(35) == 24);
    assert(math::phi(70) == 24);
    assert(math::phi(97) == 96);
    assert(math::phi(194) == 96);
    assert(math::phi(73) == 72);
    assert(math::phi(48) == 16);
    assert(math::binomial(4, 2) == 6);
    assert(math::binomial(6, 3) == 20);
    assert(math::binomial(6, 2) == 15);
    assert(math::binomial(2, 0) == 1);
    assert(math::binomial(5, 2) == 10);
    assert(math::binomial(2, 2) == 1);
    assert(math::binomial(3, 1) == 3);

    sieve::N = 40000;
    sieve::odd_sieve();
    assert(sieve::count_odd_primes(10, 20) == 4);
    assert(sieve::count_odd_primes(100, 200) == 21);
    assert(sieve::count_odd_primes(1, 9) == 3);

    return 0;
}
