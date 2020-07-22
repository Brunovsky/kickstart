#include "../math.hpp"

int main() {
    assert(power(3, 17, 5) == 3);
    assert(power(3, 17) == 129140163);
    assert(gcd(135, 54) == 27);
    assert(gcd(135, -54) == 27);
    assert(gcd(-135, -54) == 27);
    assert(totient(35) == 24);
    assert(totient(70) == 24);
    assert(totient(97) == 96);
    assert(totient(194) == 96);
    assert(totient(73) == 72);
    assert(totient(48) == 16);
    assert(binomial(4, 2) == 6);
    assert(binomial(6, 3) == 20);
    assert(binomial(6, 2) == 15);
    assert(binomial(2, 0) == 1);
    assert(binomial(5, 2) == 10);
    assert(binomial(2, 2) == 1);
    assert(binomial(3, 1) == 3);

    i64 x, y;
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

    printf("math OK\n");
    return 0;
}
