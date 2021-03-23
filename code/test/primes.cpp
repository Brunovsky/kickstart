#include "../primes.hpp"

#include "../random.hpp"
#include "test_utils.hpp"

// *****

odd_sieve sieve;

void unit_test_sieve() {
    sieve.N = 10'000'000;
    sieve.sieve();
    assert(sieve.count_odd_primes(10, 20) == 4);
    assert(sieve.count_odd_primes(100, 200) == 21);
    assert(sieve.count_odd_primes(1, 9) == 3);
    // wolfram: 1e6th prime is 15485863, 2e6th prime is 32452843
    assert(sieve.count_odd_primes(15485863, 32452843) == 1'000'001);
    // wolfram: 1.00e7th prime is 179424673, 1.05e7th prime is 188943803
    assert(sieve.count_odd_primes(179424674, 188943803) == 500'000);
    print_ok("unit test sieve");
}

void unit_test_factor() {
    static const map<ulong, vector<ulong>> factorizations = {
        {73, {73}},
        {107972737, {97, 101, 103, 107}},
        {8 * 9 * 23 * 31 * 37 * 41, {2, 2, 2, 3, 3, 23, 31, 37, 41}},
        {717, {3, 239}},
        {919, {919}},
    };

    for (auto [n, factors] : factorizations) {
        auto simple = factor_simple(n);
        auto fast = factor(n);
        sort(begin(simple), end(simple));
        sort(begin(fast), end(fast));
        assert(simple == factors);
        assert(fast == factors);
    }
    assert(miller_rabin(65537));
    assert(miller_rabin(6700417));
    assert(miller_rabin(15485863));
    assert(miller_rabin(32452843));
    assert(miller_rabin(179424673));
    assert(miller_rabin(188943803));
    print_ok("unit test factor");
}

void stress_test_factor(int T = 500) {
    intd numfactorsd(1, 30);
    vector<uint> primes = {2};
    primes.insert(end(primes), begin(sieve.odd_primes), begin(sieve.odd_primes) + 50);
    intd factord(0, primes.size() - 1);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test factor");
        vector<ulong> factors;
        ulong n = 1;
        for (int j = 0, numf = numfactorsd(mt); j < numf; j++) {
            ulong f = primes[factord(mt)];
            if (ULONG_MAX / f < n)
                break;
            n *= f;
            factors.push_back(f);
        }
        sort(begin(factors), end(factors));

        auto simple = factor_simple(n);
        auto fast = factor(n);
        sort(begin(simple), end(simple));
        sort(begin(fast), end(fast));
        assert(simple == factors);
        assert(fast == factors);
    }
    print_ok("stress test factor");
}

void stress_test_jacobi() {
    for (long n = 1; n < 300; n += 2) {
        for (long m = 1; m < 300; m += 2) {
            if (gcd(n, m) == 1) {
                int reciprocity = ((n % 4) == 3 && (m % 4) == 3) ? -1 : 1;
                if (jacobi(n, m) * jacobi(m, n) != reciprocity) {
                    fail("Jacobi computed incorrect reciprocity");
                }
            }
        }
    }
    print_ok("stress test jacobi");
}

int main() {
    unit_test_sieve();
    unit_test_factor();
    stress_test_factor();
    stress_test_jacobi();
    return 0;
}
