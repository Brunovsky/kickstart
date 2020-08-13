#include "../primes.hpp"

// *****

odd_sieve sieve;

void test_sieve() {
    sieve.N = 10'000'000;
    sieve.sieve();
    assert(sieve.count_odd_primes(10, 20) == 4);
    assert(sieve.count_odd_primes(100, 200) == 21);
    assert(sieve.count_odd_primes(1, 9) == 3);
    // wolfram: 1e6th prime is 15485863, 2e6th prime is 32452843
    assert(sieve.count_odd_primes(15485863, 32452843) == 1'000'001);
    // wolfram: 1.00e7th prime is 179424673, 1.05e7th prime is 188943803
    assert(sieve.count_odd_primes(179424674, 188943803) == 500'000);
}

map<ulong, vector<ulong>> factorizations = {
    {73, {73}},
    {107972737, {97, 101, 103, 107}},
    {8 * 9 * 23 * 31 * 37 * 41, {2, 2, 2, 3, 3, 23, 31, 37, 41}},
    {717, {3, 239}},
    {919, {919}},
};

void test_factor() {
    for (auto entry : factorizations) {
        auto simple = factor_simple(entry.first);
        auto fast = factor(entry.first);
        sort(begin(simple), end(simple));
        sort(begin(fast), end(fast));
        assert(simple == entry.second);
        assert(fast == entry.second);
    }
    assert(miller_rabin(65537));
    assert(miller_rabin(6700417));
    assert(miller_rabin(15485863));
    assert(miller_rabin(32452843));
    assert(miller_rabin(179424673));
    assert(miller_rabin(188943803));
}

void test_jacobi() {
    for (long n = 1; n < 300; n += 2) {
        for (long m = 1; m < 300; m += 2) {
            if (gcd(n, m) == 1) {
                int reciprocity = ((n % 4) == 3 && (m % 4) == 3) ? -1 : 1;
                assert(jacobi(n, m) * jacobi(m, n) == reciprocity);
            }
        }
    }
}

int main() {
    test_sieve();
    test_factor();
    test_jacobi();
    return 0;
}
