#undef NDEBUG
#include "test_utils.hpp"
#include "../numeric/math.hpp"
#include "../numeric/modnum.hpp"

void unit_test_gcd() {
    assert(gcd(135, 54) == 27);
    assert(gcd(135, -54) == 27);
    assert(gcd(-135, -54) == 27);

    long x, y;
    assert(gcd(54, 135, x, y) == 27);
    assert(54 * x + 135 * y == 27);
    assert(gcd(54, -135, x, y) == 27);
    assert(54 * x + -135 * y == 27);
    assert(gcd(-54, 135, x, y) == 27);
    assert(-54 * x + 135 * y == 27);
    assert(gcd(-54, -135, x, y) == 27);
    assert(-54 * x + -135 * y == 27);
    assert(gcd(13121, 17431, x, y) == 1);
    assert(invmod(3, 17) == 6);
    assert(invmod(4, 17) == 13);
}

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

void unit_test_modnum() {
    using MN = modnum<100>;
    MN n = 20;
    n += 31;
    assert(n == 51);
    n /= 3;
    assert(n == 17);
    n /= 7;
    assert(n == 731);
    n -= 31;
    assert(n == 0);
}

int main() {
    RUN_SHORT(unit_test_gcd());
    RUN_SHORT(unit_test_others());
    RUN_SHORT(unit_test_modsqrt());
    RUN_SHORT(unit_test_modlog());
    RUN_SHORT(unit_test_modnum());
    return 0;
}
