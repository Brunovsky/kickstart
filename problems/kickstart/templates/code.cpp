#include "disjoint_set.hpp"
#include "hash.hpp"
#include "kmp.hpp"
#include "math.hpp"
#include "sieve.hpp"
#include "fenwick.hpp"
#include "segment_tree.hpp"
#include "quad_tree.hpp"

int main() {
    assert(math::power(3, 17, 5) == 3);
    assert(math::power(3, 17) == 129140163);
    assert(math::gcd(135, 54) == 27);
    assert(math::gcd(135, -54) == 27);
    assert(math::gcd(-135, -54) == 27);
    assert(math::totient(35) == 24);
    assert(math::totient(70) == 24);
    assert(math::totient(97) == 96);
    assert(math::totient(194) == 96);
    assert(math::totient(73) == 72);
    assert(math::totient(48) == 16);
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

    math::i64 x, y;
    assert(math::gcd(54, 135, x, y) == 27);
    assert(54 * x + 135 * y == 27);
    assert(math::gcd(54, -135, x, y) == 27);
    assert(54 * x + -135 * y == 27);
    assert(math::gcd(-54, 135, x, y) == 27);
    assert(-54 * x + 135 * y == 27);
    assert(math::gcd(-54, -135, x, y) == 27);
    assert(-54 * x + -135 * y == 27);
    assert(math::gcd(13121, 17431, x, y) == 1);
    assert(math::invmod(3, 17) == 6);
    assert(math::invmod(4, 17) == 13);

    return 0;
}
