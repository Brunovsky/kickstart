#include "../numeric/primes.hpp"

#include "../numeric/sieves.hpp"
#include "../random.hpp"
#include "test_utils.hpp"

void speed_test_sieves() {
    for (int N : {31'600, 100'000, 316'000, 1'000'000, 3'160'000, 10'000'000, 31'600'000,
                  100'000'000}) {
        print(" speed test sieves N={}\n", N);

        START(classic);
        classic_sieve(N);
        TIME(classic);
        PRINT_TIME(classic);

        START(least_prime);
        least_prime_sieve(N);
        TIME(least_prime);
        PRINT_TIME(least_prime);

        START(num_prime_divisors);
        num_prime_divisors_sieve(N);
        TIME(num_prime_divisors);
        PRINT_TIME(num_prime_divisors);

        START(num_divisors);
        num_divisors_sieve(N);
        TIME(num_divisors);
        PRINT_TIME(num_divisors);

        START(sum_divisors);
        sum_divisors_sieve(N);
        TIME(sum_divisors);
        PRINT_TIME(sum_divisors);

        START(phi);
        phi_sieve(N);
        TIME(phi);
        PRINT_TIME(phi);

        START(modinv);
        modinv_sieve(N, 1'000'000'007);
        TIME(modinv);
        PRINT_TIME(modinv);
    }
}

void unit_test_classic_sieve() {
    auto primes = classic_sieve(100'000);
    assert(primes.size() == 9592u);

    assert(count_primes(10, 20, primes) == 4);
    assert(count_primes(100, 200, primes) == 21);
    assert(count_primes(1, 9, primes) == 4);
    // wolfram: 1e6th prime is 15485863, 2e6th prime is 32452843
    assert(count_primes(15485863, 32452843, primes) == 1'000'001);
    // wolfram: 1.00e7th prime is 179424673, 1.05e7th prime is 188943803
    assert(count_primes(179424674, 188943803, primes) == 500'000);
}

#define PRINT(vec)                   \
    cout << setw(15) << #vec << " "; \
    for (int n = 0; n < M; n++)      \
        cout << setw(3) << vec[n];   \
    cout << endl

void unit_test_sieves() {
    const int N = 100, M = 21;

    auto primes = classic_sieve(N);
    auto least = least_prime_sieve(N);
    auto tau_primes = num_prime_divisors_sieve(N);
    auto tau = num_divisors_sieve(N);
    auto sigma = sum_divisors_sieve(N);
    auto phi = phi_sieve(N);
    auto modinv1e9 = modinv_sieve(N, 23);

    int ans[][M] = {
        {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73},
        {0, 0, 2, 3, 2, 5, 2, 7, 2, 3, 2, 11, 2, 13, 2, 3, 2, 17, 2, 19, 2},
        {0, 0, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 2, 2, 1, 1, 2, 1, 2},
        {0, 1, 2, 2, 3, 2, 4, 2, 4, 3, 4, 2, 6, 2, 4, 4, 5, 2, 6, 2, 6},
        {0, 1, 3, 4, 7, 6, 12, 8, 15, 13, 18, 12, 28, 14, 24, 24, 31, 18, 39, 20, 42},
        {0, 1, 1, 2, 2, 4, 2, 6, 4, 6, 4, 10, 4, 12, 6, 8, 8, 16, 6, 18, 8},
        {0, 1, 12, 8, 6, 14, 4, 10, 3, 18, 7, 21, 2, 16, 5, 20, 13, 19, 9, 17, 15},
    };

    PRINT(primes), PRINT(ans[0]);
    PRINT(least), PRINT(ans[1]);
    PRINT(tau_primes), PRINT(ans[2]);
    PRINT(tau), PRINT(ans[3]);
    PRINT(sigma), PRINT(ans[4]);
    PRINT(phi), PRINT(ans[5]);
    PRINT(modinv1e9), PRINT(ans[6]);

    for (int n = 0; n < M; n++) {
        assert(primes[n] == ans[0][n]);
        assert(least[n] == ans[1][n]);
        assert(tau_primes[n] == ans[2][n]);
        assert(tau[n] == ans[3][n]);
        assert(sigma[n] == ans[4][n]);
        assert(phi[n] == ans[5][n]);
        assert(modinv1e9[n] == ans[6][n]);
    }
}

void unit_test_num_divisors_sieve() {
    const int N = 1'000'000;

    auto lp = least_prime_sieve(N);
    auto divs = num_divisors_sieve(N);

    for (int n = 2; n <= N; n++) {
        int m = n, actual = 1;
        while (m > 1) {
            int i = 0, f = lp[m];
            do {
                m /= f, i++;
            } while (lp[m] == f);
            actual *= i + 1;
        }
        assert(actual == divs[n]);
    }
}

void unit_test_factor() {
    static const map<long, vector<long>> factorizations = {
        {73, {73}},
        {107972737, {97, 101, 103, 107}},
        {8 * 9 * 23 * 31 * 37 * 41, {2, 2, 2, 3, 3, 23, 31, 37, 41}},
        {717, {3, 239}},
        {919, {919}},
    };

    for (auto [n, factors] : factorizations) {
        auto simple = factor_simple(n);
        sort(begin(simple), end(simple));
        assert(simple == factors);
    }
}

void stress_test_factor(int T = 500) {
    intd numfactorsd(1, 30);
    auto primes = classic_sieve(10'000);
    intd factord(0, primes.size() - 1);

    for (int t = 0; t < T; t++) {
        print_progress(t, T, "stress test factor");
        vector<long> factors;
        long n = 1;
        for (int j = 0, numf = numfactorsd(mt); j < numf; j++) {
            long f = primes[factord(mt)];
            if (LONG_MAX / f < n)
                break;
            n *= f;
            factors.push_back(f);
        }
        sort(begin(factors), end(factors));

        auto simple = factor_simple(n);
        sort(begin(simple), end(simple));
        assert(simple == factors);
    }
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
}

int main() {
    RUN_SHORT(unit_test_sieves());
    RUN_SHORT(unit_test_factor());
    RUN_SHORT(unit_test_num_divisors_sieve());
    RUN_BLOCK(stress_test_factor());
    RUN_BLOCK(stress_test_jacobi());
    RUN_BLOCK(speed_test_sieves());
    return 0;
}
