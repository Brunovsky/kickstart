#include "../math.hpp"

// *****

void test_gcd() {
    assert(gcd(135, 54) == 27);
    assert(gcd(135, -54) == 27);
    assert(gcd(-135, -54) == 27);

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
}

void test_others() {
    assert(power(3, 17, 5) == 3);
    assert(power(3, 17) == 129140163);
    assert(totient(35) == 24);
    assert(totient(70) == 24);
    assert(totient(97) == 96);
    assert(totient(194) == 96);
    assert(totient(73) == 72);
    assert(totient(48) == 16);
    assert(choose(4, 2) == 6);
    assert(choose(6, 3) == 20);
    assert(choose(6, 2) == 15);
    assert(choose(2, 0) == 1);
    assert(choose(5, 2) == 10);
    assert(choose(2, 2) == 1);
    assert(choose(3, 1) == 3);
}

int main() {
    test_gcd();
    test_others();
    return 0;
}
