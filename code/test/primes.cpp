#include "test_utils.hpp"
#include "../numeric/primes.hpp"
#include "../numeric/sieves.hpp"
#include "../numeric/modnum.hpp"
#include "../random.hpp"

void stress_test_jacobi() {
    for (long n = 1; n < 300; n += 2) {
        for (long m = 1; m < 300; m += 2) {
            if (gcd(n, m) == 1) {
                int reciprocity = ((n % 4) == 3 && (m % 4) == 3) ? -1 : 1;
                assert(jacobi(n, m) * jacobi(m, n) == reciprocity);
            }
        }
    }
}

void stress_test_miller_rabin() {
    static const char* what[2] = {"composite", "prime"};
    constexpr long N = 4'000'000;

    auto primes = classic_sieve(N);

    vector<bool> small_prime(N + 1, false);
    for (int p : primes) {
        small_prime[p] = true;
    }
    for (long n = 1; n <= N; n++) {
        if (small_prime[n] != miller_rabin(n)) {
            print("miller_rabin failed for n={}\n", n);
            print("expected: {}\n", what[small_prime[n]]);
            print("     got: {}\n", what[miller_rabin(n)]);
        }
        assert(small_prime[n] == miller_rabin(n));
    }
    print("small miller_rabin OK\n");

    for (int v : {5, 20, 300, 1000}) {
        long L = N * (N - v), R = N * (N - v + 5);
        auto large_primes = get_primes(L, R, primes);
        vector<bool> large_prime(R - L + 1, false);
        for (long n : large_primes) {
            large_prime[n - L] = true;
        }
        for (long n = L; n <= N; n++) {
            if (large_primes[n - L] != miller_rabin(n)) {
                print("miller_rabin failed for n={}\n", n);
                print("expected: {}\n", what[small_prime[n - L]]);
                print("     got: {}\n", what[miller_rabin(n)]);
            }
            assert(large_prime[n - L] == miller_rabin(n));
        }
        print("large miller_rabin {}..{} OK\n", L, R);
    }
}

int main() {
    RUN_BLOCK(stress_test_jacobi());
    RUN_BLOCK(stress_test_miller_rabin());
    return 0;
}
