#include "../bits.hpp"

using namespace std;

// *****

void test() {
    assert(is_power_of_two(2));
    assert(is_power_of_two(32));
    assert(!is_power_of_two(3));
    assert(!is_power_of_two(7));

    assert(lsbits(7, 3) == "111");
    assert(lsbits(13, 4) == "1011");
    assert(lsbits(0b1001001, 7) == "1001001");
    assert(lsbits(0b101011010110, 12) == "011010110101");
    assert(lsbits(0, 7) == "0000000");
    assert(lsbits(32, 7) == "0000010");
    uint v = 0b10000001000001000010001001011111;
    reverse_bits(v);
    assert(v == 0b11111010010001000010000010000001);
}

int main() {
    test();
    return 0;
}
