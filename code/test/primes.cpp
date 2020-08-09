#include "../primes.hpp"

// *****

odd_sieve sieve;

void test_sieve() {
    sieve.N = 10'000'000;
    sieve.sieve();
    assert(sieve.count_odd_primes(10, 20) == 4);
    assert(sieve.count_odd_primes(100, 200) == 21);
    assert(sieve.count_odd_primes(1, 9) == 3);
}

map<ulong, map<ulong, int>> factorizations = {
    {73, {{73, 1}}},
    {107972737, {{97, 1}, {101, 1}, {103, 1}, {107, 1}}},
};

void test_factor() {
    for (auto entry : factorizations) {
        assert(factorize(entry.first) == entry.second);
    }
}

int main() {
    test_sieve();
    test_factor();
    return 0;
}
