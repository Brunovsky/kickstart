#include "test_utils.hpp"
#include "../numeric/math.hpp"
#include "../numeric/modnum.hpp"

void unit_test_others() {
    assert(modpow(3, 17, 5) == 3);
    assert(intpow(3, 17) == 129140163);
    assert(intfac(6) == 720);
    assert(intfac(10) == 3628800);
    assert(phi(35) == 24);
    assert(phi(70) == 24);
    assert(phi(97) == 96);
    assert(phi(194) == 96);
    assert(phi(73) == 72);
    assert(phi(48) == 16);
    assert(choose(4, 2) == 6);
    assert(choose(6, 3) == 20);
    assert(choose(6, 2) == 15);
    assert(choose(2, 0) == 1);
    assert(choose(5, 2) == 10);
    assert(choose(2, 2) == 1);
    assert(choose(3, 1) == 3);
    assert(choose(43, 18) == 608359048206);
    assert(choose(58, 13) == 3155581562280);
    assert(choosemod(58, 13, 61) == 17);
    assert(partitions(20) == 627);
    assert(partitions(120) == 1844349560);
}

void unit_test_modlog() {
    assert(modlog(4, 2, 7) == 2);
    assert(modlog(4, 2, 9) == -1);
    assert(modlog(3, 1, 13) == 3);
    assert(modlog(7, 2, 19) == -1);
    assert(modlog(9, 4, 19) == 7);
    assert(modlog(6, 1, 31) == 6);

    int hit = 0, miss = 0;
    for (long p : {7, 31, 51, 73, 97, 103, 111, 131, 151, 219, 250}) {
        for (long a = 1; a < p; a++) {
            for (long b = 1; b < p; b++) {
                long x = modlog(a, b, p);
                x != -1 ? hit++ : miss++;
                if (x != -1) {
                    assert(modpow(a, x, p) == b);
                }
            }
        }
    }
    print("modlog: hit: {} | miss: {}\n", hit, miss);
}

void unit_test_modsqrt() {
    assert(modsqrt(41, 61) == 38 || modsqrt(41, 61) == 23);
    assert(modpow(38, 2, 61) == 41 && modpow(23, 2, 61) == 41);

    int hit = 0, miss = 0;
    for (long p : {7, 31, 73, 97, 103, 131, 151, 251, 313, 571, 787, 991}) {
        for (long a = 1; a < p; a++) {
            long x = modsqrt(a, p);
            x != -1 ? hit++ : miss++;
            if (x != -1) {
                assert(modpow(x, 2, p) == a && modpow(-x, 2, p) == a);
            }
        }
    }
    assert(hit == miss);
    print("modsqrt: hit: {} | miss: {}  (should be equal)\n", hit, miss);
}

#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

void stress_test_modnum() {
    constexpr int mod = 998244353;
    using num = modnum<mod>;
    using mont = montg<mod>;

    constexpr int N = 30'000, M = 30'000;
    vector<uint> a(N), b(M);
    for (uint i = 0; i < N; i++) {
        a[i] = rand_unif<uint>(1, mod - 1);
    }
    for (uint i = 0; i < M; i++) {
        b[i] = rand_unif<uint>(1, mod - 1);
    }

    num ans = 0;
    TIME_BLOCK(modnum) {
        vector<num> am(N), bm(M);
        for (int i = 0; i < N; i++)
            am[i] = num(a[i]);
        for (int j = 0; j < M; j++)
            bm[j] = num(b[j]);

        for (int i = 0; i < N; i++)
            for (int j = 0; j < M; j++)
                ans += am[i] * bm[j];
        for (int i = 0; i < N; i++)
            ans *= am[i];
        for (int j = 0; j < M; j++)
            ans *= bm[j];
    }

    TIME_BLOCK(1LL) {
        uint d = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                d += (1ULL * a[i] * b[j]) % mod;
                if (d >= mod)
                    d -= mod;
            }
        }
        for (int i = 0; i < N; i++) {
            d = 1LL * d * a[i] % mod;
        }
        for (int j = 0; j < M; j++) {
            d = 1LL * d * b[j] % mod;
        }
        assert(ans == d);
    }

    TIME_BLOCK(smul) {
        constexpr double s = (1.0) / mod;
        uint d = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                auto n = 1LL * a[i] * b[j];
                int v = n - long(n * s) * mod;
                d += v >= mod ? v - mod : v < 0 ? v + mod : v;
                if (d >= mod)
                    d -= mod;
            }
        }
        for (int i = 0; i < N; i++) {
            auto n = 1LL * d * a[i];
            int v = n - long(n * s) * mod;
            d = v >= mod ? v - mod : v < 0 ? v + mod : v;
        }
        for (int j = 0; j < M; j++) {
            auto n = 1LL * d * b[j];
            int v = n - long(n * s) * mod;
            d = v >= mod ? v - mod : v < 0 ? v + mod : v;
        }
        assert(ans == d);
    }

    TIME_BLOCK(montg) {
        print("montg r={} n2={}\n", mont::r, mont::n2);
        mont d(0);
        vector<mont> am(N), bm(M);
        for (int i = 0; i < N; i++) {
            am[i] = mont(a[i]);
        }
        for (int j = 0; j < M; j++) {
            bm[j] = mont(b[j]);
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                d += am[i] * bm[j];
            }
        }
        for (int i = 0; i < N; i++) {
            d = d * am[i];
        }
        for (int j = 0; j < M; j++) {
            d = d * bm[j];
        }
        assert(ans == d.get());
    }
}

int main() {
    RUN_SHORT(unit_test_others());
    RUN_SHORT(unit_test_modsqrt());
    RUN_SHORT(unit_test_modlog());
    RUN_SHORT(stress_test_modnum());
    return 0;
}
